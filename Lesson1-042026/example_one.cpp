#include <chrono>
#include <cstdint>
#include <iostream>

uint64_t Sum(uint64_t initial_number, uint64_t final_number) {
  uint64_t sum = 0;
  for (uint64_t number = initial_number; number <= final_number; number++) {
    sum += number;
  }
  return sum;
}

int main() {
  uint64_t number;
  std::cout << "Digite um número: ";
  std::cin >> number;

  std::chrono::time_point<std::chrono::high_resolution_clock> begin_time_point =
      std::chrono::high_resolution_clock::now();

  uint64_t total_sum;
  total_sum = Sum(1, number);

  std::chrono::time_point<std::chrono::high_resolution_clock> end_time_point =
      std::chrono::high_resolution_clock::now();

  std::chrono::duration<float> elapsed_time = (end_time_point - begin_time_point);
  std::cout << "Soma de 1 até " << number << " é: " << total_sum << std::endl;
  std::cout << "Tempo: " << elapsed_time.count() << "s" << std::endl;
}