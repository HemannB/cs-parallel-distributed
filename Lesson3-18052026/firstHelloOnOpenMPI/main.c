// Copyright
// Programação Paralela e Distribuída
// URI Santiago
// Professor Laurence

#include <mpi.h>
#include <stdio.h>
#include <string.h>

/* Constantes*/
const int kFirstProcessRank = 0;
const int kMpiMessageTag = 1;
const int kMpiMessageMaximumSize = 200;

int main(int argument_count, char* argument_value[]) {
  /* Inicializa MPI */
  int error_code = MPI_Init(&argument_count, &argument_value);
  if (error_code != MPI_SUCCESS) {
    printf("Erro ao inicializar o MPI!\n");
    MPI_Abort(MPI_COMM_WORLD, error_code);
  }

  /* Salva tempo inicial*/
  double initial_time = MPI_Wtime();

  /* Determina o ranque e número de processos */
  int curren_process_rank, number_of_process;
  MPI_Comm_rank(MPI_COMM_WORLD, &curren_process_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);

  /* Obtêm versão e nome do processador e mostra*/
  int mpi_version, mpi_subversion;
  MPI_Get_version(&mpi_version, &mpi_subversion);
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int processor_name_length;
  MPI_Get_processor_name(processor_name, &processor_name_length);
  printf("Eu sou o processo %d estou em %s com MPI %d.%d\n", curren_process_rank, processor_name,
         mpi_version, mpi_subversion);

  if (curren_process_rank != kFirstProcessRank) {
    /* Processos diferentes do primeiro enviam uma mensagem para o primeiro */
    char mpi_message[kMpiMessageMaximumSize];
    snprintf(mpi_message, kMpiMessageMaximumSize, "Oi!");
    MPI_Send(mpi_message, (strlen(mpi_message) + 1), MPI_CHAR, kFirstProcessRank, kMpiMessageTag,
             MPI_COMM_WORLD);
  } else {
    /* Primeiro processo recebe mensagens dos demais processos */
    for (int rank_source = 1; rank_source < number_of_process; rank_source++) {
      MPI_Status mpi_message_status;
      int mpi_message_length;
      MPI_Probe(rank_source, kMpiMessageTag, MPI_COMM_WORLD, &mpi_message_status);
      MPI_Get_count(&mpi_message_status, MPI_CHAR, &mpi_message_length);

      char mpi_message[kMpiMessageMaximumSize];
      MPI_Recv(mpi_message, mpi_message_length, MPI_CHAR, rank_source, kMpiMessageTag,
               MPI_COMM_WORLD, &mpi_message_status);
      printf("Eu sou o processo 0 e recebi \"%s\" do processo %d com a tag %d\n", mpi_message,
             mpi_message_status.MPI_SOURCE, mpi_message_status.MPI_TAG);
    }
  }

  /* Finaliza MPI */
  MPI_Finalize();

  /* Salva tempo final */
  double final_time = MPI_Wtime();

  /* Mostra tempo de execução*/
  printf("Eu sou o processo %d demorei %fs para executar\n", curren_process_rank,
         (final_time - initial_time));
}
