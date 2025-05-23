Card Management & Shared Memory Communication System
Overview
This project combines a Card Management System with a Shared Memory IPC (Inter-Process Communication)
demo using C programming. It provides a CLI-based interface for managing customer 
card data via hash tables and demonstrates data transfer between processes via shared memory and semaphores.

Components
1. hashing.c - Card Management System
Implements a secure, password-protected system to:

Add new customer data and generate random card details (Visa, MasterCard, AMEX, MeToo bank).

Store and retrieve customer info using hash tables (by card number and by name).

Support full CRUD operations:

Add

Search by card or name

Delete by card or name

View all records (password protected)

Persist data to a file (customerdata.txt)

Validate input for names, dates, and emails.

2. shared.c - Shared Memory IPC Demo
Demonstrates:

Reading data from source.txt in the parent process.

Writing data to destination.txt in the child process using System V shared memory and POSIX semaphores.

Full synchronization using sem_wait/sem_post.

3. run.c - Execution Controller
Runs both:

hashing (card management)

shared (IPC demonstration)
Sequentially and handles error checking for system calls.

Build Instructions
Make sure you have GCC installed. Then compile all files:

bash
Copy
Edit
gcc hashing.c -o hashing
gcc shared.c -o shared -pthread
gcc run.c -o run
Note: -pthread is needed for semaphore operations.

Run
bash
Copy
Edit
./run
Files
hashing.c – Main system for card/customer data management

shared.c – Shared memory and semaphore IPC example

run.c – Launcher to sequentially run both programs

customerdata.txt – Stores persistent customer information

source.txt – Input file for shared memory demonstration

destination.txt – Output file written by the child process

Features Summary
🔐 Secure access with password protection

🧠 Efficient search using hash tables

📇 Card generation logic tailored per bank

🧪 Input validation for data integrity

🧵 Synchronized inter-process communication

💾 Persistent storage of customer data

Author
Developed by Abhishek Kumar Sharma
For queries or contributions, feel free to raise issues or fork the repo.
