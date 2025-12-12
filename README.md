**Project Name:** Design and Development of a Compiler for the "WHI" Language
**Environment/Languages:** C Language, VS Code
**Role:** Individual Developer (Design, Implementation, Testing)

**[Project Overview]**
Developed a compiler from scratch using C that translates the high-level language "WHI" into intermediate code executable by a simple stack-based abstract machine.

**[Key Implementations]**
* **Lexical & Syntax Analysis:**
    * Implemented a lexical analyzer based on Deterministic Finite Automata (DFA).
    * Constructed a syntax analyzer using Recursive Descent Parsing for the grammar defined in BNF.
* **Intermediate Code Generation:**
    * Implemented translation logic for stack machine instruction sets (e.g., `add`, `sub`, `lod`, `sto`, `jmp`).
    * Adopted the **backpatching** technique using linked lists to resolve jump addresses for control structures like `if` statements and `while` loops.
* **Memory Management:**
    * Managed symbol tables and tokens using dynamic memory allocation (`malloc`/`realloc`) to ensure efficient resource usage.

**[Results & Testing]**
* Verified the correct compilation and execution of complex algorithms, including the "Chicken and Rabbit" problem, systems of linear equations, primality tests, and GCD/LCM calculations, on the abstract machine.

**[Challenges & Key Takeaways]**
* **Architectural Improvement:** Initially faced challenges with high coupling between the syntax analysis and code generation modules, which made debugging difficult. This experience highlighted the importance of "High Cohesion, Low Coupling" in software design.
* **Performance Optimization:** Learned that frequent dynamic memory allocation can be a bottleneck, gaining insights into optimization strategies such as batch memory allocation.
