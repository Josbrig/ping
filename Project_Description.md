# Project Description – Continuous Ping Statistics System

## 1. Overview

This project aims to develop a **cross-platform C++20 application** that performs continuous ICMP-based ping measurements to one or more IP addresses and generates **live statistical analyses** of their response times.

The program continuously monitors network latency, builds long-term statistics for each address individually, and provides real-time visualizations to aid in performance monitoring and network diagnostics.

---

## 2. Objectives

The primary goal is to design and implement an efficient, reliable, and extensible **ping monitoring tool** that supports long-term data collection and analysis. It should:

- Continuously measure ICMP response times from one or more IP addresses.
- Generate and maintain **individual statistics** for each target.
- Provide both **real-time updates** and **historical insights**.
- Visualize trends, distributions, and variations in network latency.

---

## 3. Functional Requirements

### 3.1 Measurement and Data Capture
- Perform continuous ICMP ping operations to multiple IP targets.
- Record each response’s timestamp and duration.
- Handle timeouts, packet loss, and unreachable hosts gracefully.

### 3.2 Statistical Analysis
- Compute key metrics for each IP address:
  - **Minimum (min)** response time  
  - **Maximum (max)** response time  
  - **Arithmetic mean (average)**  
  - **Median value**
- Generate a time-based performance curve.
- Build a **histogram** of response times to show distribution and variance.
- Continuously update all metrics during the measurement process.

### 3.3 Visualization & Output
- Present results dynamically and continuously.
- Support multiple display formats:
  - Text-based (console/stdout)
  - Optional graphical visualization using OpenCV
- Enable easy export of historical data for further analysis (CSV or JSON).

---

## 4. Technical Requirements

| Category | Requirement |
|-----------|--------------|
| Programming language | C++20 |
| Build management | CMake |
| Dependencies | OpenCV (for visualization, fetched via CMake) |
| Supported operating systems | Linux, macOS, Windows (MinGW, WSL, Cygwin) |
| Compiler compatibility | GCC, Clang, MSVC |
| Linking | Static linking for all required libraries |

CMake configuration must ensure that:
- Only C++ source files are built (no tests, demos, or mixed languages).
- OpenCV is integrated via FetchContent and includes `opencv_contrib`.
- All necessary include paths are properly defined.

---

## 5. System Architecture

The project architecture is class-based and type-safe.

### Core Components
- **PingManager** – controls measurement sessions and IP groups.  
- **PingWorker** – executes ICMP pings and collects response data.  
- **StatisticsCollector** – computes ongoing metrics per IP.  
- **Visualizer** – generates live charts and histograms (OpenCV).  
- **DataStorage** – stores persistent results for long-term analysis.

Each module adheres to object-oriented design principles and follows Doxygen documentation standards.

---

## 6. Performance and Robustness

- Continuous execution requires stable handling of network fluctuations.
- The system should efficiently manage large amounts of data without excessive memory usage.
- All numeric operations use type-safe C++ variable declarations.
- Proper synchronization is required when measuring multiple IPs concurrently.

---

## 7. Build and Deployment

### Build Process
1. Clone repository.  
2. Configure build using CMake:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
