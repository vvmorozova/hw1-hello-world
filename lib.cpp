#include "lib.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

std::vector<std::string> split(const std::string &str, char d) {
  std::vector<std::string> r;

  std::string::size_type start = 0;
  std::string::size_type stop = str.find_first_of(d);
  while (stop != std::string::npos) {
    r.push_back(str.substr(start, stop - start));

    start = stop + 1;
    stop = str.find_first_of(d, start);
  }

  r.push_back(str.substr(start));

  return r;
}

void print_ips(const std::vector<std::vector<std::string>> &ip_pool,
                std::function<bool(const std::vector<std::string> &)> canBePrint = nullptr)
{
  for (auto currIP : ip_pool) {

    if (canBePrint) {
      if (!canBePrint(currIP)) {
        continue;
      }
    }

      int i = 0;
      for (auto currTet : currIP) {
        std::cout << currTet;
        if (i++ < 3) {
          std::cout << ".";
        }
      }
      std::cout << std::endl;
    }
}

int ip_filter() {
  try {

    std::vector<std::vector<std::string>> ip_pool;

    for (std::string line; std::getline(std::cin, line);) {
      std::vector<std::string> str = split(line, ' ');
      ip_pool.push_back(split(str.at(0), '.'));
    }

    auto compFunc = [](std::vector<std::string> fisrtIP,
                       std::vector<std::string> secondIP) -> bool {
      for (int i = 0; i < 4; i++) {
        int diff = std::stoi(fisrtIP[i]) - std::stoi(secondIP[i]);
        if (diff != 0) {
          return diff > 0 ? true : false;
        }
      }
      return true;
    };
    std::sort(ip_pool.begin(), ip_pool.end(), compFunc);
    std::cout << "ip_pool len" << ip_pool.size() << std::endl;

    //print_ips(ip_pool);
    
    auto firstByteIs1 = [](std::vector<std::string> ip) -> bool {
      if (ip[0] == "1") {
        return true;
      }
      return false;
    };
    print_ips(ip_pool, firstByteIs1);
    

  } catch (const std::exception &e) {
    std::cerr << "err func name: " << e.what() << std::endl;
  }

  return 0;
}
