#include "lib.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <array>

#define BYTES_IN_IPV4 4
#define IP_PLACE_IN_INPUT_STR 0

using IP_t = std::array<u_int8_t, 4>;

// string std is split into vector of strings by delimiter d
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

//converts std::string to IP_t = std::array<u_int8_t, 4>
IP_t strToIP_t(const std::vector<std::string> &splitted) {
  IP_t ip;
  for (auto i = 0; i < BYTES_IN_IPV4; i++) {
    ip[i] = static_cast<u_int8_t>(std::stoi(splitted[i]));
  }
  return ip;
}

// if an IP address from the IP pool satisfies the canBePrint condition, it is printed to stdout
// if canBePrint is not specified, all ips are printed
void print_ips(const std::vector<IP_t> &ip_pool,
                std::function<bool(const IP_t &)> canBePrint = nullptr)
{
  for (const auto &currIP : ip_pool) {

    if (canBePrint && !canBePrint(currIP)) {
      continue;
    }
    for (auto i = 0; i < BYTES_IN_IPV4; i++) {
      std::cout << static_cast<int>(currIP[i]);
      if (i < BYTES_IN_IPV4 - 1) {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
  }
}

int ip_filter() {
  try {

    std::vector<IP_t> ip_pool;

    for (std::string line; std::getline(std::cin, line);) {
      std::vector<std::string> str = split(line, '\t');
      auto currIP = strToIP_t(split(str.at(IP_PLACE_IN_INPUT_STR), '.'));
      ip_pool.push_back(currIP);
    }

    std::sort(ip_pool.begin(), ip_pool.end(), std::greater<IP_t>());
    print_ips(ip_pool);
    
    auto firstByteIs1 = [](const IP_t &ip) -> bool {
      return ip[0] == 1;
    };
    print_ips(ip_pool, firstByteIs1);
    
    auto f46s70 = [](const IP_t &ip) -> bool {
      return ip[0] == 46 && ip[1] == 70;
    };
    print_ips(ip_pool, f46s70);

    auto any46 = [](const IP_t &ip) -> bool {
      for (int i = 0; i < BYTES_IN_IPV4; i++) {
        if (ip[i] == 46) {
          return true;
        }
      }
      return false;
    };
    print_ips(ip_pool, any46);

  } catch (const std::exception &e) {
    std::cerr << "err func name: " << e.what() << std::endl;
  }

  return 0;
}
