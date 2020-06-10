<p align="center">
  <img src="https://raw.githubusercontent.com/thzoid/vscode-nyancat/master/images/icon_small.png">
</p>

# nyancat Changelog

## nyancat H

### Added 19 quantum and classical operations:
 - end
 - mov
 - put
 - cmp
 - je
 - jne
 - jg
 - jge
 - jl
 - jle
 - add
 - sub
 - mul
 - div
 - m
 - h
 - x
 - y
 - z

### Added arguments
Data can be passed to the algorithm and used as a constant, with a friendly name, inside the code.

### Added Labels
Labels can be declared and referenced so the pointer will "jump" to them, backwards or forwards.

### Added Classical pointers
Classical registers can be referenced using the `!` notation.

### Added Reserved pointers
Reserved (classical) registers can be referenced using the `%` notation.

### Added Quantum pointers
Quantum registers can be referenced using the `?` notation.