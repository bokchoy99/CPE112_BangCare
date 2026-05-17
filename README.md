# 🏥 BangCare: Hospital Triage Management System
![Version](https://img.shields.io/badge/Version-1.1-008080?style=for-the-badge)
![Language](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active-32CD32?style=for-the-badge)

**BangCare** is a hospital triage management system designed to manage bed allocation workflows and optimize limited medical resources such as hospital beds and physician availability. The system is implemented in C and focuses on low-complexity data processing to reduce patient waiting time and improve emergency response efficiency.

---

## 🚩 Problem Statement
In traditional hospital settings, manual triage or naive sequential processing (First-Come, First-Served) often leads to:
* **Critical Delays:** High-severity patients waiting behind stable patients.
* **Wait-Time Deterioration:** Patients' conditions worsening while in the queue without a dynamic update system.

## 🎯 Objectives
* **Reduced Queue Overhead:** Use optimized data structures to ensure the highest-priority patients are treated first.
* **Minimize Waiting Time:** Implement a **Dynamic Aging System** to prevent patients from "stuck" in the queue.
* **Efficient Bed Management:** Automate matching between patient severity and bed types (ER/OPD).
* **Memory Safety:** Ensure zero memory leaks or dangling pointers in a resource-constrained environment.

---

## 🧠 Data Structure Used
| Data Structure | Component |
| :--- | :--- |
| **1. Hash Table** | **Patient Record** |
| **2. Priority Queue (Heap)** | **Triage System** |
| **3. Linked List** | **Aging System** |
| **4. Doubly Linked List** | **Bed Management** |

### 📊 Performance Analysis
| Operation | Data Structure | Complexity |
| :--- | :--- | :---: |
| **Patient Search** | Hash Table | $O(1)$ |
| **Triage Insertion** | Binary Heap | $O(\log N)$ |
| **Dynamic Aging** | Linked List + Heap | $O(N + \log N)$ |
| **Bed Allocation** | Doubly Linked List | $O(1)$ |

## 🏗️ Project Structure
* `📁 core/`
    * `main.c`
    * `config.h`
* `📁 system/`
* `📁 patient/`
* `📁 bed/`
* `📁 data/`
    * `hash_table.c`
    * `heap.c`
    * `linked_list.c`
* `📁 ui/`
* `📁 utils/`
* `📁 data_output/`

---

## 🛠 Getting Started
### Prerequisites
* GCC Compiler (C99 or higher)
* Standard C Libraries

### Installing
1. Clone the repository:
```bash
   git clone https://github.com/bokchoy99/CPE112_BangCare.git
```

## ✏️ Usage CMD
```bash
  ### Patient Management
  add <name> <severity> <pain>
  peek hash <id>
  peek aging
  peek sev <num>
  
  ### Time Simulation
  tick <n>
  tick rec <n>

  ### System Information
  stat
  cmd
  about
  exit
```

---

## 🍒 Contributions

| Name | Student ID | Responsibility |
| :--- | :--- | :--- |
| Ms. Karnsinee Nikrotamethanee | 68070503412 | Hash Table, CMD Interface |
| Ms. Nichapa Piromying | 68070503422 | System Module, Priority Queue (Heap) |
| Ms. Patchara Nimittakunchai | 68070503475 | Aging List (Linked List) |
| Ms. Patteera Pattaraporntaweewat | 68070503478 | Bed Management (Doubly Linked List) |

### 🔍 Responsibility Details

#### 🗂️ Ms. Karnsinee Nikrotamethanee — Hash Table & CMD Interface
Responsible for implementing the **Hash Table** used as the core patient record system, enabling $O(1)$ average-case lookup by patient ID. Also responsible for the **CMD interface**, handling all user commands and input parsing for the system.

#### ⚙️ Ms. Nichapa Piromying — System Module & Priority Queue
Responsible for the overall **system module** and the **Priority Queue (Binary Heap)** that drives the triage engine. The heap ensures that the highest-severity patients are always dequeued first with $O(\log N)$ insertion and extraction.

#### ⏳ Ms. Patchara Nimittakunchai — Aging List
Responsible for the **Dynamic Aging System** implemented as a **Linked List**. This module tracks how long each patient has waited in the queue and dynamically elevates their priority to prevent indefinite waiting.

#### 🛏️ Ms. Patteera Pattaraporntaweewat — Bed Management
Responsible for the **Bed Management** module implemented using a **Doubly Linked List**, handling allocation and deallocation of ER and OPD beds in $O(1)$ time and ensuring efficient matching between patient severity and available bed types.

---

* This Project is part of The CPE112: 
* University: King Mongkut's University of Technology Thonburi