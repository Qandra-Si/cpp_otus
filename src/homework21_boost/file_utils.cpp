#include <iostream>
#include <fstream>
#include <array>
#include <functional>

#include "file_utils.h"

#include <boost/crc.hpp> // boost::crc_32_type
#include <boost/uuid/detail/md5.hpp> // boost::uuids::detail::md5


namespace homework21 {

// в этом файле есть набор методов, которые ориентированы на выбор алгоритма хеширования:
//  - get_hash_size
//  - get_crc32
//  - get_md5
// а также есть методы, которые не ориентированы на алгоритм, но работают с типом результата хеш-суммы:
//  - byte_array_to_string
//  - hash_to_string(uint32_t)
//  - hash_to_string(array<uint32_t,4>)
// а также есть методы, которые получают функтор для использования указанного алгоритма хеширования:
//  - read_first_block
//  - compare_files
// методы которые являются интерфейсными:
//  - get_hash_crc32
//  - get_hash_md5
//  - compare_files_crc32
//  - compare_files_md5

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

std::string byte_array_to_string(const uint8_t* _first, const uint8_t* _last)
{
  std::string res;
  static char hexval[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
  for (const uint8_t* i = _first; i != _last; ++i)
  {
    res.append(1, hexval[((*i >> 4) & 0xf)]);
    res.append(1, hexval[(*i) & 0x0f]);
  }
  return res;
}

std::string hash_to_string(const uint32_t& hash)
{
  return byte_array_to_string(reinterpret_cast<const uint8_t*>(&hash), reinterpret_cast<const uint8_t*>(&hash) + sizeof(hash));
}

std::string hash_to_string(const std::array<uint32_t,4>& hash)
{
  return byte_array_to_string(reinterpret_cast<const uint8_t*>(&hash[0]), reinterpret_cast<const uint8_t*>(&hash[0]) + sizeof(hash[0])*hash.size());
}

template<class Hash>
Hash read_first_block(
  const std::string& fname,
  file_size_t fsize,
  unsigned fblock,
  unsigned hsize,
  std::function<Hash(const void* p, unsigned sz)> calc)
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

  return calc(&block[0], block_size);
}

uint32_t get_hash_crc32(const std::string& fname, file_size_t fsize, unsigned fblock)
{
  const unsigned hash_size = get_hash_size(algorithm_t::crc32);
  return read_first_block<uint32_t>(fname, fsize, fblock, hash_size, get_crc32);
}

std::array<uint32_t, 4> get_hash_md5(const std::string& fname, file_size_t fsize, unsigned fblock)
{
  const unsigned hash_size = get_hash_size(algorithm_t::md5);
  return read_first_block<std::array<uint32_t,4>>(fname, fsize, fblock, hash_size, get_md5);
}

template<class Hash>
struct file_descriptor_t
{
  using hash_t = typename Hash;
  using rest_blocks_t = std::vector<hash_t>;
  rest_blocks_t hash;
  std::fstream fs;
  std::string fname;
  int index{-1};
};

template<class Hash>
std::vector<filenames_t> compare_files(
  const filenames_t& fnames,
  bool verbose,
  file_size_t fsize,
  unsigned fblock,
  unsigned hsize,
  std::function<Hash(const void* p, unsigned sz)> calc)
{
  // если размер файла меньше размера блока, то файлы УЖЕ идентичны, возвращаем входной список
  if (fsize <= fblock) return std::vector<filenames_t>{fnames};

  using hash_t = typename Hash;
  using descriptor_t = typename file_descriptor_t<hash_t>;
  using descriptors_t = typename std::vector<file_descriptor_t<hash_t>>;

  // готовим буферы для хранения хешей (на один меньше, т.к. ранее уже вычитывался первый блок)
  const unsigned rest_blocks = (unsigned)((fsize + fblock - 1) / fblock - 1);
  descriptors_t descriptors;
  descriptors.reserve(fnames.size()); // готовим группы хешей по кол-ву файлов на входе
  for (auto& fname : fnames)
  {
    descriptor_t& d = descriptors.emplace_back(descriptor_t());
    d.fs.open(fname.c_str(), std::ios::binary | std::ios::in);
    if (!d.fs.is_open()) continue; // файл уже удалён?!
    d.fs.seekg(fblock, d.fs.beg);
    d.hash.reserve(rest_blocks); // готовим хеши для каждого из файлов
    d.fname = fname;
    d.index = -1;
  }

  // список имён файлов с одинаковым содержимым
  std::vector<filenames_t> equal;

  for (descriptors_t::iterator i1 = descriptors.begin(), end = descriptors.end(); i1 != end; ++i1)
  {
    descriptor_t& f1desc = *i1;
    // сравниваем файлы каждый с каждым, но стараемся читать с диска как можно меньше данных
    // все блоки хешируем и сравниваем их попарно
    for (descriptors_t::iterator i2 = i1 + 1; i2 != end; ++i2)
    {
      // чтобы сравнить файлы надо иметь достаточное кол-во считанных блоков с диска
      descriptor_t& f2desc = *i2;
      // если все хеши для файлов уже были загружены и сравнения завешались успешны,
      // то проверяем принадлежность файлов группам - это самая быстрая операция, без
      // перебора хеш-сумм
      if ((f1desc.index != f2desc.index) && (f1desc.index >= 0) && (f2desc.index >= 0))
      {
        // просто вывод отладки (если включена)
        if (verbose) std::cout << "files are differs by groups #" << f1desc.index << " and #" << f2desc.index << " of " << f1desc.fname << " " << f2desc.fname << std::endl;
        continue;
      }
      for (unsigned idx = 0; idx < rest_blocks; ++idx)
      {
        // подгружаем недостающие блоки с диска
        bool f1need = f1desc.hash.size() <= idx;
        bool f2need = f2desc.hash.size() <= idx;
        if (f1need || f2need)
        {
          const unsigned block_size = (fsize < (file_size_t)((idx+1)*fblock)) ? (unsigned)((idx+1)*fblock - fsize) : fblock;
          std::vector<uint8_t> block;
          if (block_size < hsize)
            block.resize(hsize, 0);
          else
            block.resize(block_size);
          if (f1need)
          {
            f1desc.fs.read(reinterpret_cast<char*>(&block[0]), block_size);
            f1desc.hash.push_back(calc(&block[0], block_size));
            // просто вывод отладки (если включена)
            if (verbose) std::cout << "one more block#" << f1desc.hash.size() << " " << hash_to_string(f1desc.hash.back()) << " read from file " << f1desc.fname << std::endl;
          }
          if (f2need)
          {
            f2desc.fs.read(reinterpret_cast<char*>(&block[0]), block_size);
            f2desc.hash.push_back(calc(&block[0], block_size));
            // просто вывод отладки (если включена)
            if (verbose) std::cout << "one more block#" << f2desc.hash.size() << " " << hash_to_string(f2desc.hash.back()) << " read from file " << f2desc.fname << std::endl;
          }
        }
        // попарно сравниваем хеши загруженных блоков
        if (f1desc.hash[idx] != f2desc.hash[idx])
        {
          // просто вывод отладки (если включена)
          if (verbose) std::cout << "files on block#" << idx+1 << " are different " << f1desc.fname << " " << f2desc.fname << std::endl;
          break;
        }
        else
        {
          // просто вывод отладки (если включена)
          if (verbose) std::cout << "files on block#" << idx+1 << " are still same " << f1desc.fname << " " << f2desc.fname << std::endl;
        }
        // если есть ещё что загружать и сравнивать, - продолжаем
        if ((idx+1) != rest_blocks) continue;
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
        // просто вывод отладки (если включена)
        if (verbose) std::cout << "files are EQUAL in group#" << f1desc.index << " " << f1desc.fname << " " << f2desc.fname << std::endl;
      }
    }
  }

  return equal;
}

std::vector<filenames_t> compare_files_crc32(const filenames_t& fnames, file_size_t fsize, unsigned fblock, bool verbose)
{
  const unsigned hash_size = get_hash_size(algorithm_t::crc32);
  return compare_files<uint32_t>(fnames, verbose, fsize, fblock, hash_size, get_crc32);
}

std::vector<filenames_t> compare_files_md5(const filenames_t& fnames, file_size_t fsize, unsigned fblock, bool verbose)
{
  const unsigned hash_size = get_hash_size(algorithm_t::md5);
  return compare_files<std::array<uint32_t,4>>(fnames, verbose, fsize, fblock, hash_size, get_md5);
}

}
