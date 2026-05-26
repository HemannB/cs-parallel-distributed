#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

uint64_t SumDigitsOfNumber(uint64_t number)
{
    uint64_t sum = 0;

    while (number > 0)
    {
        sum += number % 10;
        number /= 10;
    }

    return sum;
}

uint64_t Sum(uint64_t initial_number, uint64_t final_number)
{
    if (initial_number > final_number)
    {
        return 0;
    }

    uint64_t sum = 0;
    uint64_t current_digit_sum = SumDigitsOfNumber(initial_number);

    for (uint64_t number = initial_number; number <= final_number; number++)
    {
        sum += current_digit_sum;

        uint64_t current_number = number;
        current_digit_sum++;

        while (current_number % 10 == 9)
        {
            current_digit_sum -= 9;
            current_number /= 10;
        }
    }

    return sum;
}

int main()
{
    uint64_t number;
    uint32_t num_threads;

    std::cout << "Digite um número: ";
    std::cin >> number;
    std::cout << "Digite o número de threads: ";
    std::cin >> num_threads;

    if (num_threads == 0)
    {
        std::cout << "O número de threads deve ser maior que zero." << std::endl;
        return 1;
    }

    if (number == 0)
    {
        std::cout << "Soma dos algarismos de 0 até 0 é: 0" << std::endl;
        std::cout << "Tempo: 0s" << std::endl;
        return 0;
    }

    if (num_threads > number)
    {
        num_threads = static_cast<uint32_t>(number);
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> begin_time_point =
        std::chrono::high_resolution_clock::now();

    uint64_t chunk = number / num_threads;
    uint64_t resto = number % num_threads;
    std::vector<uint64_t> results(num_threads, 0);
    std::vector<std::thread> threads(num_threads);

    uint64_t start = 1;
    for (uint32_t i = 0; i < num_threads; i++)
    {
        uint64_t current_chunk = chunk + (i < resto ? 1 : 0);
        uint64_t end = start + current_chunk - 1;
        threads[i] = std::thread([&results, i, start, end]()
                                 { results[i] = Sum(start, end); });
        start = end + 1;
    }

    for (uint32_t i = 0; i < num_threads; i++)
    {
        threads[i].join();
    }

    uint64_t total_sum = 0;
    for (uint32_t i = 0; i < num_threads; i++)
    {
        total_sum += results[i];
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> end_time_point =
        std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_time = (end_time_point - begin_time_point);
    std::cout << "Soma dos algarismos de 0 até " << number << " é: " << total_sum << std::endl;
    std::cout << "Tempo: " << elapsed_time.count() << "s" << std::endl;
}
