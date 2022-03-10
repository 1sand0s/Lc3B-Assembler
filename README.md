# Lc3B-Assembler
![GitHub Workflow Status](https://img.shields.io/github/workflow/status/1sand0s/Lc3B-Assembler/Makefile%20CI)
![GitHub](https://img.shields.io/github/license/1sand0s/Lc3B-Assembler)
![GitHub](https://img.shields.io/badge/Compliance-MISRA--C:2004-green.svg)

<h2> What is it ?</h2>

A standalone assembler for the [Lc3B ISA](https://courses.engr.illinois.edu/ece411/fa2019/mp/LC3b_ISA.pdf) written from scratch in C without any 
external libraries besides std Clibs. 
<br>The computer can only process 1's and 0's or machine code and therefore any program instructing 
the computer to perform a certain task must be in machine code. However, writing programs in machine code is extremely difficult. This 
difficulty is overcome by introducing higher level programming languages which abstract away the complexity of coding directly in machine code. 
Assembly language is one such higher(relative to machine code) level of abstraction. However, since the computer can only understand machine code, 
programs written in these higher level languages must be transformed into machine code. An assembler is a tool that transforms programs written in 
the assembly language to machine code. 

<h2>Setup</h2>

1. <h4>Clone the Lc3B-Assembler repository:</h4>

    If you installed Git, execute the following command:<br> `git clone https://github.com/1sand0s/Lc3B-Assembler.git`<br><br>
    Otherwise, download the repository as a [zip ball](https://github.com/1sand0s/Lc3B-Assembler/archive/refs/heads/main.zip)

2. <h4>Building and Running Tests:</h4>

    `cd` to the project root directory `Lc3B-Assembler`, and execute
    ```
    ./configure LIBS=-lm
    make 
    make check
    sudo make install
    ```

    The `configure` script might complain about [check](https://libcheck.github.io/check/) package not being present. `check` is the test runner used for 
    unit-testing. You can install `check` as follows `sudo apt-get install check` or build it from source by following these [instructions](https://github.com/libcheck/check)

3. <h4>Running:</h4>

    To use the assembler, `cd` to the directory `Lc3B-Assembler/src` and execute:<br> `./Lc3BAssembler <*.asm> <*.hex>`
