# 🖧 Cluster Open MPI — Soma Paralela

> Configuração de um cluster com Open MPI para execução de programas paralelos entre múltiplas máquinas, com exemplo prático de soma paralela de 1 até 5 bilhões.

---

## 📋 Conteúdo

- [Visão Geral](#visão-geral)
- [Pré-requisitos](#pré-requisitos)
- [Passo a Passo](#passo-a-passo)
  - [1. Instalar o Open MPI](#1-instalar-o-open-mpi)
  - [2. Descobrir os IPs das máquinas](#2-descobrir-os-ips-das-máquinas)
  - [3. Verificar conectividade (Ping)](#3-verificar-conectividade-ping)
  - [4. Configurar SSH com senha](#4-configurar-ssh-com-senha)
  - [5. Configurar SSH com chave RSA (sem senha)](#5-configurar-ssh-com-chave-rsa-sem-senha)
  - [6. Compilar o programa MPI](#6-compilar-o-programa-mpi)
  - [7. Copiar o binário para todas as máquinas](#7-copiar-o-binário-para-todas-as-máquinas)
  - [8. Criar o Hostfile e executar](#8-criar-o-hostfile-e-executar)
- [Exemplo: Soma Paralela](#exemplo-soma-paralela)
- [Configurações de Ambiente](#configurações-de-ambiente)
  - [WSL (Windows)](#wsl-windows)
  - [VirtualBox](#virtualbox)
- [Resultado Esperado](#resultado-esperado)

---

## Visão Geral

Este projeto demonstra como montar um **cluster Open MPI** com duas ou mais máquinas e executar um programa de soma paralela. Cada processo calcula uma faixa de valores, e os resultados são combinados no processo raiz (rank 0).

```
Máquina A  ──────────────────────  Máquina B
[processo 0] [processo 1]    [processo 2] [processo 3]
     └──────────── MPI_Reduce ────────────┘
                   Resultado final
```

---

## Pré-requisitos

- 2 ou mais máquinas (físicas, VMs ou WSL) na **mesma rede local**
- Sistema operacional **Linux** (ou WSL no Windows)
- Acesso à internet para instalação de pacotes

---

## Passo a Passo

### 1. Instalar o Open MPI

Execute em **todas as máquinas** do cluster:

```bash
sudo apt update
sudo apt install openmpi-bin libopenmpi-dev
```

Verifique a instalação:

```bash
mpirun --version
```

---

### 2. Descobrir os IPs das máquinas

**Windows:**
```cmd
ipconfig -all
```

**macOS / Linux / WSL:**
```bash
ip a
```

Anote o IP de cada máquina (geralmente no formato `192.168.x.x`).

---

### 3. Verificar conectividade (Ping)

De cada máquina, faça ping nas demais para confirmar que estão na mesma rede:

```bash
ping <IP da outra máquina>
```

> ⚠️ Se o ping falhar no WSL/Windows, veja a seção [Configurações de Ambiente](#configurações-de-ambiente).

---

### 4. Configurar SSH com senha

Instale o servidor SSH em **todas as máquinas**:

```bash
sudo apt update
sudo apt install openssh-server
```

Teste a conexão (substituindo pelos valores reais):

```bash
# Conectar
ssh <usuário>@<IP>

# Desconectar
exit
```

---

### 5. Configurar SSH com chave RSA (sem senha)

Para que o MPI possa se conectar automaticamente entre as máquinas, é necessário autenticação sem senha.

**Em cada máquina**, gere um par de chaves RSA:

```bash
ssh-keygen -t rsa -b 4096
# Pressione Enter em todas as perguntas (sem passphrase)
```

**Troque as chaves entre as máquinas** (execute na Máquina A, apontando para a Máquina B, e vice-versa):

```bash
scp ~/.ssh/id_rsa.pub <usuário>@<IP da outra máquina>:~/.ssh/authorized_keys
```

Teste se a conexão SSH agora funciona **sem pedir senha**:

```bash
ssh <usuário>@<IP da outra máquina>
```

---

### 6. Compilar o programa MPI

Em **uma das máquinas** (a que vai orquestrar a execução), compile o programa:

```bash
mpicc -o soma_paralela soma_paralela.c -lm
```

---

### 7. Copiar o binário para todas as máquinas

O binário compilado deve estar no **mesmo caminho** em todas as máquinas:

```bash
scp soma_paralela <usuário>@<IP da outra máquina>:~/soma_paralela
```

> 💡 O caminho de destino deve ser idêntico em todas as máquinas (ex: `~/soma_paralela`).

---

### 8. Criar o Hostfile e executar

Crie um arquivo chamado `hostfile` na máquina principal:

```
<usuário_maquina1>@<IP_maquina1>:<slots>
<usuário_maquina2>@<IP_maquina2>:<slots>
```

**Exemplo com 2 processos por máquina (4 no total):**

```
alice@192.168.1.10:2
alice@192.168.1.11:2
```

Execute o programa no cluster:

```bash
mpiexec --hostfile hostfile -np 4 ./soma_paralela
```

| Parâmetro | Descrição |
|-----------|-----------|
| `--hostfile` | Arquivo com a lista de máquinas e slots |
| `-np` | Número total de processos MPI |

---

## Exemplo: Soma Paralela

### Código (`soma_paralela.c`)

```c
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    long long n = 5000000000LL;
    long long local_sum = 0;
    long long total_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start = MPI_Wtime();

    long long chunk = n / size;
    long long inicio = (long long)rank * chunk + 1;
    long long fim    = (rank == size - 1) ? n : inicio + chunk - 1;

    for (long long i = inicio; i <= fim; i++) {
        local_sum += i;
    }

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0) {
        printf("Soma de 1 até %lld = %lld\n", n, total_sum);
        printf("Tempo: %.3fs\n", end - start);
    }

    MPI_Finalize();
    return 0;
}
```

### Como funciona

Cada processo recebe uma **faixa de números** para somar:

```
n = 5.000.000.000   |   4 processos

Processo 0: soma de          1 até  1.250.000.000
Processo 1: soma de 1.250.000.001 até  2.500.000.000
Processo 2: soma de 2.500.000.001 até  3.750.000.000
Processo 3: soma de 3.750.000.001 até  5.000.000.000
                                   ↓
                           MPI_Reduce (soma total)
                                   ↓
                     Processo 0 exibe o resultado
```

---

## Configurações de Ambiente

### WSL (Windows)

Para que o WSL consiga se comunicar com outras máquinas na rede local, configure o **Modo Mirrored** de rede:

1. Abra as **Configurações do WSL**
2. Vá em **Rede**
3. Em **Modo de rede**, selecione **Mirrored**

Além disso, desabilite o Firewall do Windows para a rede em uso:

1. Abra **Segurança do Windows**
2. Vá em **Firewall e proteção de rede**
3. Selecione a rede ativa e **desative o Microsoft Defender Firewall**

> ⚠️ Reabilite o firewall ao terminar o laboratório.

---

### VirtualBox

Para que VMs no VirtualBox consigam se comunicar entre si e com o host na rede local:

1. Nas configurações da VM, vá em **Rede**
2. No **Adaptador 1**, em **Conectado a**, selecione **Placa em modo Bridge**
3. Repita para todas as VMs do cluster

---

## Resultado Esperado

```
Soma de 1 até 5000000000 = 12500000002500000000
Tempo: 0.427s
```

> O resultado corresponde à fórmula `n * (n + 1) / 2` com `n = 5.000.000.000`.

---

## 📚 Referências

- [Open MPI — Documentação Oficial](https://www.open-mpi.org/doc/)
- SILVA, Gabriel P.; BIANCHINI, Calebe P.; COSTA, Evaldo B. *Programação Paralela e Distribuída: Com MPI, OpenMP e OpenACC para Computação de Alto Desempenho*. Casa do Código, 2022.

---

> Disciplina: **Programação Paralela e Distribuída** — URI Campus Santiago  
> Professor: Laurence Crestani Tasca