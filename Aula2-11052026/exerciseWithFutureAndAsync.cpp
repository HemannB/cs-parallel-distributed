#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <numeric>
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
    const uint64_t number = 5000000000;
    const int executions = 10;
    const std::vector<uint32_t> thread_options = {1, 2, 4, 8, 16, 32};

    for (uint32_t num_threads : thread_options)
    {
        std::vector<double> times;
        times.reserve(executions);
        uint64_t total_sum = 0;

        uint64_t slice = number / num_threads;
        uint64_t resto = number % num_threads;

        std::cout << "\nTestando com " << num_threads << " thread(s)" << std::endl;

        for (int execution = 0; execution < executions; execution++)
        {
            std::chrono::time_point<std::chrono::high_resolution_clock> begin_time_point =
                std::chrono::high_resolution_clock::now();

            std::vector<std::future<uint64_t>> futures(num_threads);

            for (uint32_t i = 0; i < num_threads; i++)
            {
                uint64_t start = i * slice + 1;
                uint64_t end = (i == num_threads - 1) ? start + slice - 1 + resto : start + slice - 1;
                futures[i] = std::async(std::launch::async, Sum, start, end);
            }

            total_sum = 0;
            for (uint32_t i = 0; i < num_threads; i++)
            {
                total_sum += futures[i].get();
            }

            std::chrono::time_point<std::chrono::high_resolution_clock> end_time_point =
                std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> elapsed_time = end_time_point - begin_time_point;
            times.push_back(elapsed_time.count());

            std::cout << "Execução " << execution + 1 << ": ";
            std::cout << "soma dos algarismos de 0 até " << number << " = " << total_sum;
            std::cout << " | tempo = " << elapsed_time.count() << "s" << std::endl;
        }

        double average_time = std::accumulate(times.begin(), times.end(), 0.0) / times.size();

        std::cout << "Média com " << num_threads << " thread(s): ";
        std::cout << average_time << "s" << std::endl;
    }
}
