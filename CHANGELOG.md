<p align="center">
  <img src="https://raw.githubusercontent.com/thzoid/vscode-nyancat/master/images/icon_small.png">
</p>

# nyancat changelog

## nyancat He

#### Added classical pointer referencing
Values of classical registers can be referenced and passed as plain values parameters through pointer referencing notation: `[0!]` or `[0%]`.

#### Removed 1 classical task:
 - put.

#### Added 1 classical task:
 - jmp.

#### Parameters types reformulation for the following tasks:
 - end;
   - Value, reference or argument.
 - mov;
   - Classical register;
   - Value, reference or argument.
 - cmp;
   - Classical register;
   - Value, reference or argument.
 - add;
   - Classical register;
   - Value, reference or argument.
 - sub;
   - Classical register;
   - Value, reference or argument.
 - mul;
   - Classical register;
   - Value, reference or argument.
 - div.
   - Classical register;
   - Value, reference or argument.

## nyancat H

#### Added 19 quantum and classical tasks:
 - end;
 - mov;
 - put;
 - cmp;
 - je;
 - jne;
 - jg;
 - jge;
 - jl;
 - jle;
 - add;
 - sub;
 - mul;
 - div;
 - m;
 - h;
 - x;
 - y;
 - z.

#### Added arguments
Data can be passed to the algorithm and used as a constant, with a friendly name, inside the code.

#### Added Labels
Labels can be declared and referenced so the pointer will "jump" to them, backwards or forwards.

#### Added Classical pointers
Classical registers can be referenced by using the `!` notation.

#### Added Reserved pointers
Reserved (classical) registers can be referenced by using the `%` notation.

#### Added Quantum pointers
Quantum registers can be referenced by using the `?` notation.