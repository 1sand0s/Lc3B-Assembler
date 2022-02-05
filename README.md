# Lc3B-Assembler
![GitHub Workflow Status](https://img.shields.io/github/workflow/status/1sand0s/Lc3B-Assembler/Makefile%20CI)
![GitHub](https://img.shields.io/github/license/1sand0s/Lc3B-Assembler)

Assembler for the LC3b ISA

<h2>How to build</h2>


1. <h4>Install Git:</h4>

    Download [Git](http://git-scm.com/downloads) and install it.

2. <h4>Clone the Lc3B-Assembler repository:</h4>

    If you installed Git, execute the following command: `https://github.com/1sand0s/Lc3B-Assembler.git`<br>
    Otherwise, download the repository as a [zip ball](https://github.com/1sand0s/Lc3B-Assembler/archive/refs/heads/main.zip)

6. <h4>Building Lc3B-Assembler:</h4>

    Open a command prompt/terminal, `cd` to the directory `Lc3B-Assembler`, and execute
    ```
    ./configure LIBS=-lm
    make install
    ```

7. <h4>Running:</h4>

    To use the assembler, `cd` to the directory `Lc3B-Assembler/src` and execute `./a.out <*.asm> <*.hex>`
