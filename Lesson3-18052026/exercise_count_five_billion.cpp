/*
* Programação Paralela e Distribuída
* URI Santiago
* Bruno Hemann
*/
#include <mpi.h>
#include <algorithm>
#include <cstdint>
#include <iostream>

// Constantes
const int kFirstProcessRank = 0;
const std::uint64_t kDefaultNumber = 5000000000;

std::uint64_t Sum(std::uint64_t initial_number, std::uint64_t final_number) {
  if (initial_number > final_number) {
    return 0;
  }

  const std::uint64_t quantity = final_number - initial_number + 1;
  const std::uint64_t extremes_sum = initial_number + final_number;

  if (quantity % 2 == 0) {
    return (quantity / 2) * extremes_sum;
  }

  return quantity * (extremes_sum / 2);
}

int main(int argument_count, char* argument_value[]) {
  // Inicializa MPI
  int error_code = MPI_Init(&argument_count, &argument_value);
  if (error_code != MPI_SUCCESS) {
    std::cout << "Erro ao inicializar o MPI!" << std::endl;
    MPI_Abort(MPI_COMM_WORLD, error_code);
  }

  // Salva tempo inicial
  double initial_time = MPI_Wtime();

  // Determina o ranque e número de processos
  int current_process_rank, number_of_process;
  MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);

  // Obtém versão e nome do processador e mostra
  int mpi_version, mpi_subversion;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int processor_name_length;
  MPI_Get_version(&mpi_version, &mpi_subversion);
  MPI_Get_processor_name(processor_name, &processor_name_length);
  std::cout << "Eu sou o processo " << current_process_rank << " estou em " << processor_name
            << " com MPI " << mpi_version << "." << mpi_subversion << std::endl;

  // Valor fixo do exercício: soma de 1 até 5 bilhões.
  const std::uint64_t number = kDefaultNumber;

  // Calcula o intervalo de cada processo
  const std::uint64_t slice = number / number_of_process;
  const std::uint64_t resto = number % number_of_process;
  const std::uint64_t process_rank = current_process_rank;

  const std::uint64_t extra_numbers = process_rank < resto ? 1 : 0;
  const std::uint64_t numbers_before =
      process_rank * slice + std::min(process_rank, resto);
  const std::uint64_t start = numbers_before + 1;
  const std::uint64_t end = start + slice + extra_numbers - 1;

  // Cada processo calcula sua soma parcial
  std::uint64_t partial_sum = Sum(start, end);
  std::cout << "Eu sou o processo " << current_process_rank
            << " calculei de " << start << " até " << end
            << " soma parcial = " << partial_sum << std::endl;

  // Reduz todas as somas parciais no processo 0.
  std::uint64_t total_sum = 0;
  MPI_Reduce(&partial_sum, &total_sum, 1, MPI_UINT64_T, MPI_SUM, kFirstProcessRank,
             MPI_COMM_WORLD);

  if (current_process_rank == kFirstProcessRank) {
    double final_time = MPI_Wtime();
    std::cout << "Soma de 1 até " << number << " é: " << total_sum << std::endl;
    std::cout << "Tempo: " << (final_time - initial_time) << "s" << std::endl;
  }

  double final_time = MPI_Wtime();
  std::cout << "Eu sou o processo " << current_process_rank << " demorei "
            << (final_time - initial_time) << "s para executar" << std::endl;

  // Finaliza MPI
  MPI_Finalize();
}
