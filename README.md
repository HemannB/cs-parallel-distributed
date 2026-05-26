# 🖥️ Parallel and Distributed Computing

> Labs, assignments and resources for the **Parallel and Distributed Programming** course.  
> URI — Campus Santiago | Computer Science | 2026/1  
> Professor Me.: Laurence Crestani Tasca

---

## 📁 Structure

| Folder | Date | Topic |
|--------|------|-------|
| [Lesson1-042026](./Lesson1-042026/) | Apr 2026 | Introduction & Processes/Threads |
| [Lesson2-11052026](./Lesson2-11052026/) | May 11, 2026 | Parallel digit sum benchmark — `std::async` + Amdahl/Gustafson analysis |
| [Lesson3-18052026](./Lesson3-18052026/) | May 18, 2026 | Parallel sum with MPI (`-np 4`, local processes) |
| [Lesson4-25052026](./Lesson4-25052026/) | May 25, 2026 | Open MPI cluster setup + parallel sum across machines |

---

## 📚 Lessons

### Lesson 1 — Introduction
Processes, threads and an overview of parallel and distributed architectures (UMA, NUMA, clusters, GPU/NPU accelerators).

---

### Lesson 2 — Parallel Benchmark with `std::async`
Digit sum of all numbers from 1 to 5,000,000,000 using C++ `std::async`.  
The program runs 10 executions for each thread count `{1, 2, 4, 8, 16, 32}` and records average time, enabling speedup calculation via **Amdahl's Law** and **Gustafson-Barsis's Law**.

```
Threads:  1  →  baseline
Threads:  2  →  ~2x speedup
Threads:  4  →  ~4x speedup
...
```

**Compile & run:**
```bash
g++ -O2 -std=c++17 -pthread -o benchmark benchmark.cpp
./benchmark
```

---

### Lesson 3 — MPI with Local Processes
Parallel sum from 1 to 5,000,000,000 using **Open MPI** on a single machine with multiple processes (`-np 4`).  
Each process computes a slice of the range using the arithmetic sum formula and results are combined with `MPI_Reduce`.

**Compile & run:**
```bash
mpi++ file.cpp -o exercise_count_five_billion.cpp
mpiexec -np 4 ./sum
```

**Expected output:**
```
Soma de 1 até 5000000000 é: 12500000002500000000
Tempo: ~0.4s
```

---

### Lesson 4 — Open MPI Cluster
Setting up a real **multi-machine MPI cluster** and running the parallel sum distributed across nodes.

**Cluster setup steps:**
1. Install Open MPI on all machines
2. Discover machine IPs (`ip a` / `ipconfig -all`)
3. Verify connectivity (`ping`)
4. Configure SSH with password
5. Configure SSH with RSA key (passwordless)
6. Compile the binary on one machine
7. Copy the binary to all machines (`scp`)
8. Run with a hostfile

**Run on cluster:**
```bash
mpiexec --hostfile hostfile -np 4 ./sum
```

**Hostfile format:**
```
user@192.168.1.10:2
user@192.168.1.11:2
```

See [`Lesson4-25052026/README.md`](./Lesson4-25052026/README.md) for the full cluster setup guide.

---

## 🛠️ Requirements

- **C++17** compiler (`g++`)
- **Open MPI** (`sudo apt install openmpi-bin libopenmpi-dev`)
- **SSH** (`sudo apt install openssh-server`)
- Linux / WSL / macOS

---

## 📖 References

- SILVA, G. P.; BIANCHINI, C. P.; COSTA, E. B. *Programação Paralela e Distribuída: Com MPI, OpenMP e OpenACC para Computação de Alto Desempenho*. Casa do Código, 2022.
- MIZRAHI, V. V. *Treinamento em Linguagem C*, 2ª ed. 2008.
- [Professor Lau on Youtube](https://www.youtube.com/@professor-lau)
- [Open MPI Documentation](https://www.open-mpi.org/doc/)