#include <fstream>
#include <array>

#include "file_utils.h"

#include <boost/crc.hpp> // boost::crc_32_type
#include <boost/uuid/detail/md5.hpp> // boost::uuids::detail::md5


namespace homework21 {

unsigned get_hash_size(algorithm_t algorithm)
{
  switch (algorithm)
  {
  case algorithm_t::crc32: return 4;
  case algorithm_t::md5: return 16;
  }
  assert(0 && "unsupported hash algorithm");
  return 0;
}

uint32_t get_crc32(const void* p, unsigned sz)
{
  boost::crc_32_type hash;
  hash.process_bytes(p, sz);
  return hash.checksum();
}

std::array<uint32_t,4> get_md5(const void* p, unsigned sz)
{
  boost::uuids::detail::md5 hash;
  std::array<uint32_t,4> res;
  hash.process_bytes(p, sz);
  hash.get_digest(*reinterpret_cast<boost::uuids::detail::md5::digest_type*>(&res[0]));
  return res;
}

std::vector<uint8_t> read_first_block(const std::string& fname, file_size_t fsize, unsigned fblock, unsigned hsize)
{
  assert(fsize);
  assert(fblock);

  std::fstream fs(fname.c_str(), std::ios::binary | std::ios::in);
  std::vector<uint8_t> block;

  const unsigned block_size = (fsize <= (file_size_t)fblock) ? (unsigned)fsize : fblock;
  if (block_size < hsize)
    block.resize(hsize, 0);
  else
    block.resize(block_size);

  std::streampos pos = fs.tellg();
  fs.read(reinterpret_cast<char*>(&block[0]), block_size);
  std::streampos rcvd = fs.tellg() - pos;
  assert(rcvd == block_size);

  return block;
}

uint32_t get_hash_crc32(const std::string& fname, file_size_t fsize, unsigned fblock)
{
  const unsigned hash_size = get_hash_size(algorithm_t::crc32);
  std::vector<uint8_t> block = read_first_block(fname, fsize, fblock, hash_size);
  return get_crc32(&block[0], block.size());
}

std::array<uint32_t, 4> get_hash_md5(const std::string& fname, file_size_t fsize, unsigned fblock)
{
  const unsigned hash_size = get_hash_size(algorithm_t::md5);
  std::vector<uint8_t> block = read_first_block(fname, fsize, fblock, hash_size);
  return get_md5(&block[0], block.size());
}

struct file_descriptor_t
{
  using hash_t = uint32_t;
  using rest_blocks_t = std::vector<hash_t>;
  rest_blocks_t hash;
  std::fstream fs;
  std::string fname;
  int index;
};
using file_descriptors_t = std::vector<file_descriptor_t>;

std::vector<filenames_t> compare_files_crc32(const filenames_t& fnames, file_size_t fsize, unsigned fblock)
{
  if (fsize <= fblock) return std::vector<filenames_t>{fnames};

  // готовим буферы для хранения хешей (на один меньше, т.к. ранее уже вычитывался первый блок)
  const unsigned rest_blocks = (unsigned)((fsize + fblock - 1) / fblock - 1);
  file_descriptors_t descriptors;
  descriptors.reserve(fnames.size()); // готовим группы хешей по кол-ву файлов на входе
  for (auto& fname : fnames)
  {
    file_descriptor_t& d = descriptors.emplace_back(file_descriptor_t());
    d.fs.open(fname.c_str(), std::ios::binary | std::ios::in);
    if (!d.fs.is_open()) continue; // файл уже удалён?!
    d.fs.seekg(fblock, d.fs.beg);
    d.hash.reserve(rest_blocks); // готовим хеши для каждого из файлов
    d.fname = fname;
    d.index = -1;
  }

  // список имён файлов с одинаковым содержимым
  std::vector<filenames_t> equal;

  const unsigned hash_size = get_hash_size(algorithm_t::crc32);
  for (file_descriptors_t::iterator i1 = descriptors.begin(), end = descriptors.end(); i1 != end; ++i1)
  {
    file_descriptor_t& f1desc = *i1;
    // сравниваем файлы каждый с каждым, но стараемся читать с диска как можно меньше данных
    // все блоки хешируем и сравниваем их попарно
    for (file_descriptors_t::iterator i2 = i1 + 1; i2 != end; ++i2)
    {
      // чтобы сравнить файлы надо иметь достаточное кол-во считанных блоков с диска
      file_descriptor_t& f2desc = *i2;
      for (unsigned idx = 0; idx < rest_blocks; ++idx)
      {
        // подгружаем недостающие блоки с диска
        bool f1need = f1desc.hash.size() <= idx;
        bool f2need = f2desc.hash.size() <= idx;
        if (f1need || f2need)
        {
          const unsigned block_size = (fsize < (file_size_t)((idx+1)*fblock)) ? (unsigned)((idx+1)*fblock - fsize) : fblock;
          std::vector<uint8_t> block;
          if (block_size < hash_size)
            block.resize(hash_size, 0);
          else
            block.resize(block_size);
          if (f1need)
          {
            f1desc.fs.read(reinterpret_cast<char*>(&block[0]), block_size);
            f1desc.hash.push_back(get_crc32(&block[0], block_size));
          }
          if (f2need)
          {
            f2desc.fs.read(reinterpret_cast<char*>(&block[0]), block_size);
            f2desc.hash.push_back(get_crc32(&block[0], block_size));
          }
        }
        // попарно сравниваем хеши загруженных блоков
        if (f1desc.hash[idx] != f2desc.hash[idx]) break;
        // если есть ещё что загружать и сравнивать, - продолжаем
        if (f1desc.hash.size() != rest_blocks) continue;
        // файлы прочитаны и они одинаковы (с точки зрения формулировки в домашнем задании,
        // т.к. хеш - это корзина, в которую потенциально могут попасть разное содержимое)
        if (f1desc.index >= 0)
        {
          f2desc.fs.close();
          equal[f1desc.index].push_back(f2desc.fname);
          f2desc.index = f1desc.index;
        }
        else if (f2desc.index >= 0)
        {
          f1desc.fs.close();
          equal[f2desc.index].push_back(f1desc.fname);
          f1desc.index = f2desc.index;
        }
        else
        {
          f1desc.fs.close();
          f2desc.fs.close();
          f1desc.index = f2desc.index = equal.size();
          equal.push_back(filenames_t());
          equal.back().push_back(f1desc.fname);
          equal.back().push_back(f2desc.fname);
        }
      }
    }
  }

  return equal;
}

std::vector<filenames_t> compare_files_md5(const filenames_t& fnames, file_size_t fsize, unsigned fblock)
{
  if (fsize <= fblock) return std::vector<filenames_t>{fnames};
  const unsigned hash_size = get_hash_size(algorithm_t::md5);
  return std::vector<filenames_t>();
}

}
