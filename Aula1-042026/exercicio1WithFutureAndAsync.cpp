#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <vector>

uint64_t Sum(uint64_t initial_number, uint64_t final_number)
{
    uint64_t sum = 0;
    for (uint64_t number = initial_number; number <= final_number; number++)
    {
        sum += number;
    }
    return sum;
}

int main()
{
    uint64_t number;
    uint8_t num_threads;
    std::cout << "Digite um número: ";
    std::cin >> number;
    std::cout << "Digite o número de threads: ";
    std::cin >> num_threads;

    std::chrono::time_point<std::chrono::high_resolution_clock> begin_time_point =
        std::chrono::high_resolution_clock::now();

    uint64_t slice = number / num_threads;
    uint64_t resto = number % num_threads;

    std::vector<std::future<uint64_t>> futures(num_threads);

    for (int i = 0; i < num_threads; i++)
    {
        uint64_t start = i * slice + 1;
        uint64_t end = (i == num_threads - 1) ? start + slice - 1 + resto : start + slice - 1;
        futures[i] = std::async(std::launch::async, Sum, start, end);
    }

    uint64_t total_sum = 0;
    for (int i = 0; i < num_threads; i++)
    {
        total_sum += futures[i].get();
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> end_time_point =
        std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> elapsed_time = (end_time_point - begin_time_point);
    std::cout << "Soma de 1 até " << number << " é: " << total_sum << std::endl;
    std::cout << "Tempo: " << elapsed_time.count() << "s" << std::endl;
}