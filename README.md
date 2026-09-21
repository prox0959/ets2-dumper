# ETS2 Process Memory Dumper (C++)

[![C++](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://en.cppreference.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20x64-0078D6.svg)](https://microsoft.com)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Status](https://img.shields.io/badge/Status-Completed-success.svg)](#)

A high-performance C++ utility designed for reverse engineering, memory scanning, and runtime telemetry inspection of Euro Truck Simulator 2 (x64). It attaches to the target game process, resolves virtual memory segments, and dumps target structures for security research and offline inspection.

---

## [EN] Technical Overview
* **Process Handle Acquisition:** Opens the target process with required memory query permissions (`PROCESS_VM_READ | PROCESS_QUERY_INFORMATION`).
* **Memory Traversal:** Walks virtual address descriptors via `VirtualQueryEx` to identify committed executable and writable memory regions.
* **Pattern / Signature Scanning:** Fast memory pattern matching to locate dynamic base offsets, structs, and pointers.
* **Safe Memory Dumping:** Streams memory chunks to local binary/text outputs without causing memory access violations or process crashes.

---

## [TR] Teknik Açıklama
Euro Truck Simulator 2 (x64) süreci üzerinde tersine mühendislik ve bellek yapısı araştırması yapmak için geliştirilmiş C++ aracıdır.
* **Bellek Segmenti Tarama:** `VirtualQueryEx` ile oyunun sanal bellek alanlarını ve modül offset'lerini listeler.
* **Veri Okuma:** `ReadProcessMemory` API'si ile hedef veri yapılarını ve bellek alanlarını diske dump eder.
* **Kullanım Amacı:** Yalnızca tek oyunculu modda oyun mimarisi, struct adresleme ve tersine mühendislik temellerini anlamak için geliştirilmiştir.
