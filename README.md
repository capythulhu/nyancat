<p align="center">
  <img src="https://raw.githubusercontent.com/thzoid/vscode-nyancat/master/icon.png">
</p>

# Welcome to nyancat!
**nyancat** is a C library that allows you to *simulate a quantum processor* and easily *write machine code for it!*

You can either run simple isolated quantum algorithms with `nyancat.exe` or integrate many algorithms into a C project and make more complex computations.

:warning: Please note that this library _only simulates a QPU_. You shouldn't expect it neither to be more performatic than classical computing nor to compile code that would run on a real quantum processor. This library has it's own (simplified) architecture implementation.

## Recommended IDE
I recommend [Visual Studio Code](https://code.visualstudio.com/) for you to write your quantum algorithms. I've prepared an extension that you might want to use so you can have stuff like syntax highlighting and snippets! Get it here: [nyancat VSCode Extension](http://).

# Understanding the memory architecture
Here is how the QPU (Quantum Processing Unit) implemented on this library works:

When you allocate a new quantum driver, you have to specify a number of classical registers (4 bytes/`sizeof(int)` each) and quantum registers (1 qubit each). 

:warning: It is possible to directly modify any classical, reserved or quantum register by acessing the `driver` structure in C, but it isn't recommended because that's not how a quantum computer works. Unless you're trying to implement new features on the driver itself, try to code as if your `driver` is actually calling a hardware, and it knows what it does.

For example, let's say that we allocate a driver with 3 classical registers and 2 quantum registers.

The driver's quantum memory would look like this:

| Id | Type             | Access         |
|----|------------------|----------------|
| 0  | Quantum Register | Measure and perform spin operations. |
| 1  | Quantum Register | Measure and perform spin operations. |

And the classical memory, like this:

| Id | Type                        | Access                           |
|----|-----------------------------|----------------------------------|
| 0  | Classical Register          | Read and write.                   |
| 1  | Classical Register          | Read and write.                   |
| 2  | Classical Register          | Read and write.                   |
| 3  | Task Return Register | Read and write only with specific tasks. |
| 4  | Algorithm Return Register | Read and write only with the `end` task.                        |

As you can see, there is two additional registers allocated at the end of our driver's classical memory. They are intended for being used by the driver "tasks" only &mdash; but you can access them if you want to perform more specific operations. The constant `RESERVED_REGS` refers to how many of them will be allocated on your driver's memory. You can access the address of a reserved register by adding it's relative id to that constant.

We will go into what the reserved registers are used for and their limitations. But first, let's talk about what are the driver "tasks" and how they use these registers.

# Writing quantum machine code
The implemented lexer will read machine-code-like (monolithic) algorithms from `.nya` files, but they should follow some syntax rules.

## Labels
Labels are one of the most basic and important structures on a monolithic code. They are words that denote lines on the code to which you're able to jump to. You can return to a certain label, or advance to another one, depending on a specified condition.

Here is how a label is declared in nyancat code:

`Foo`

I mean, it seems basic (and it is) but look closer. **A label declaration must always start with an upper-case letter.** We recommend you to use Pascal Case. You can reference labels that weren't declared before your current line, as long as every label has an unique name. **There can only be one label declaration per line.**

You can jump through labels using jump tasks, of which we'll talk about very soon.

## Arguments
Not everything is a constant. Let's say that you want to perform a certain quantum calculation for a certain number. How would you pass that value to your beautiful algorithm? Well, they're called your algorithm "arguments", and in nyancat code, they are declared this way:

`< foo, bar >`

**Arguments must start with a lower-case letter.** We recommend you to use Camel Case.

In contrast with labels, it is not possible to reference an argument that wasn't declared previously.

You can declare many arguments, many times, but **only once per line.** Trying to open another pair of angle brackets (`<>`) on the same line will cause your code to not work.

## Values and Pointers
Right now, the only plain values that you can write on your code are numeric, discrete values. You can write signed integers up to 4 bytes (`sizeof(int)` in C).

:warning: Just like a plain number, an argument reference is considered a literal numeric value.

However, differently from C, there are three types of pointers.

### Classical Pointer

A classical pointer is an unsigned integer that **represents an address of a non-reserved classical register.** That means that if you've allocated a driver that has 4 classical registers, it will only support algorithms that have pointers from "0" to "3".

You can represent a classical pointer in nyancat code like this:

`0!`

Where `0` is the classical register id that you want to reference.

### Reserved Pointer

A reserved (classical) pointer is an unsigned integer that **represents an address of a reserved classical register.** As mentioned early, you can always know how many reserved registers will be allocated on your driver by using the constant `RESERVED_REGS` on your C code. If your driver has only 2 allocated reserved registers, trying to run an algorithm that has a reserved pointer that isn't "0" or "1" will fail.

You can represent a classical pointer in nyancat code like this:

`0%`

Where `0` is the reserved register id that you want to reference.

:warning: Although reserved pointers have some special conditions, they are still interpreted as classical pointers.

### Quantum Pointer

A quantum pointer is an unsigned integer that **represents an address of a quantum register.** If your driver has only 3 quantum registers, an execution error will be thrown if it finds a quantum pointer outside the range "0" - "2".

You can represent a quantum pointer in nyancat code like this:

`0?`

Where `0` is the quantum register id that you want to reference.

## Tasks
Tasks (a.k.a. assembly's instructions) are calls to native functions that run classical or quantum operations on registers.

They're not named "instructions" because they don't talk to actual hardware, so I believe they're somehow different from our good old machine code instructions. Tasks can also return values and receive implicit and explicit parameters.

### Anatomy of a task
Consider the following task:

`put 1! 5`

The string `put` is what addresses what task you're calling. Each task has different parameters count and types &mdash; this one reads a classical register first and then a value &mdash; and the parameters are separated by a whitespace character. **There can only be one task per line.**

### Return values
Many tasks return values. That means they write values into the TRR (a.k.a. Task Return Register or `0%`). It might seem strange at first, but this can be extremely useful when you're writing sequential tasks.

There is an exception though (like everything in life), that is the `end` task. This task is the only one that actually writes value into the ARR (a.k.a. Algorithm Return Register or `1%`). This happens because the job of the ARR is to hold the return code of the whole algorithm. There is where the result of all your quantum magic stuff is kept after the calculations are done!

### Parameters

As mentioned, every task will ask for at least one parameter. However, because of the quantum magic, some of the information passed to a task can be invisible to our eyes! But hold on, let's start with the simplest ones.

#### Explicit Parameters

Explicit parameters are the values we pass to a task by writing them into the front of the task's name. Tasks ask for specific parameters types (pointers, values, labels, etc.) in a specific order. You can check out every available task and their parameters on the [tasks table](#All-of-them) below.

#### Implicit Parameters

These are the invisible parameters, but unfortunately, it has actually nothing to do with quantum magic. Some tasks are intended to have not much more than one way to be used. That's why, to avoid redundant code, some tasks have implicit parameters. They read the value on the TRR (`0%`), which is usually the return value of a previous task.

If you want a task to read a specific value as an implicit parameter, you'll have to force that behavior by using `put` to store the desired value into the `0%` register. However, that's not a very common situation. Double check your code, mate!

### All of them

| Name | Description                                                                               | Explicit Parameters                   | Inputs from TRR | Outputs to TRR |
|------|-------------------------------------------------------------------------------------------|---------------------------------------|-----------------|----------------|
| end  | Moves the value of the classical register to the ARR.                                     | classical_pointer                     | No              | No             |
| mov  | Moves a value from the second classical register to the first one.                        | classical_pointer1 classical_pointer2 | No              | No             |
| put  | Inserts the provided value into the specified classical register.                         | classical_pointer value               | No              | No             |
| cmp  | Compares the provided value with the TRR value and returns the result.                    | value                                 | Yes             | Yes            |
| je   | Jumps to a specified label if a previous `cmp` outputted equality.                        | label                                 | Yes             | No             |
| jne  | Jumps to a specified label if a previous `cmp` outputted inequality.                      | label                                 | Yes             | No             |
| jg   | Jumps to a specified label if a previous `cmp` outputted positive difference.             | label                                 | Yes             | No             |
| jge  | Jumps to a specified label if a previous `cmp` outputted equality or positive difference. | label                                 | Yes             | No             |
| jl   | Jumps to a specified label if a previous `cmp` outputted negative difference.             | label                                 | Yes             | No             |
| jle  | Jumps to a specified label if a previous `cmp` outputted equality or negative difference. | label                                 | Yes             | No             |
| m    | Measures the specified quantum register and returns the result.                           | quantum_pointer                       | No              | Yes            |
| h    | Performs the Hadamard gate on the specified quantum register.                             | quantum_pointer                       | No              | No             |
| x    | Performs the Pauli's X gate on the specified quantum register.                            | quantum_pointer                       | No              | No             |
| y    | Performs the Pauli's Y gate on the specified quantum register.                            | quantum_pointer                       | No              | No             |
| z    | Performs the Pauli's Z gate on the specified quantum register.                            | quantum_pointer                       | No              | No             |
