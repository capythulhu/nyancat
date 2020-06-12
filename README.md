<p align="center">
  <img src="https://raw.githubusercontent.com/thzoid/vscode-nyancat/master/images/icon_small.png">
</p>

# Welcome to nyancat!
**nyancat Algorithm Runner** is a C library that allows you to *simulate a quantum processor* and easily *write machine code for it!*

The latest stable version available for the [nyancat Algorithm Runner (nAR)](https://github.com/thzoid/nyancat/releases) supports **nyancat He**. You can download this and previous versions on the "Releases" tab.

You can either run simple isolated quantum algorithms with `nyancat.exe` or integrate many algorithms into a C project and make more complete computations.

:warning: Please note that this library _only simulates a QPU (Quantum Processing Unit)_. You shouldn't expect it neither to be more performatic than classical computing nor to compile code that would run on a real quantum processor. This library has it's own (simplified) processor architecture implementation.

## Recommended IDE
I recommend [Visual Studio Code](https://code.visualstudio.com/) for you to write your quantum algorithms. I've prepared an extension that you might want to use so you can have stuff like syntax highlighting and snippets! Get it here: [nyancat VSCode Extension](http://).

# Understanding the memory architecture
Here is how the QPU (Quantum Processing Unit) implemented on this library works:

When you allocate a new quantum driver, you have to specify a number of classical registers (4 bytes/`sizeof(int)` each) and quantum registers (1 qubit each). 

:warning: It is possible to directly modify any classical, reserved or quantum register by acessing the `driver` structure in C code, but this isn't recommended because that's not how a quantum computer works. Unless you're trying to implement new features on the driver itself, try to code as if your `driver` is actually talking to a hardware, and it knows what it does.

For example, let's say that we allocate a driver with 3 classical registers and 2 quantum registers.

The driver's quantum memory would look like this:

| ID | Type             | Access         |
|----|------------------|----------------|
| 0  | Quantum Register | Measure and perform spin operations. |
| 1  | Quantum Register | Measure and perform spin operations. |

And the classical memory, like this:

| ID | Type                        | Access                           |
|----|-----------------------------|----------------------------------|
| 0  | Classical Register          | Read and write.                   |
| 1  | Classical Register          | Read and write.                   |
| 2  | Classical Register          | Read and write.                   |
| 3  | Task Return Register | Read and write only with specific tasks. |
| 4  | Algorithm Return Register | Read and write only with the `end` task.                        |

As you can see, there are two additional registers allocated at the end of our driver's classical memory. They are intended for being used by the driver's "tasks" only &mdash; but you can access them if you want to perform more specific operations. The constant `RESERVED_REGS` refers to how many of them will take place on your driver's memory. You can access the address of a reserved register by adding it's relative ID to that constant.

We will go into what reserved registers are used for and their limitations, but first, let's talk about what are driver's "tasks" and how they use these registers.

# Writing quantum machine code
The implemented lexer will read machine-code-like (monolithic) algorithms from `.nya` files, but they must follow some syntax rules.

## Labels
Labels are one of the most basic and important structures on a monolithic code. They are words that denote lines on the code to which you're able to "jump". You return to a certain label, or advance to another one, depending on a specified condition.

Here is how a label is declared in nyancat code:

`Foo`

I mean, it seems basic (and it is) but look closer. **A label declaration must always start with an upper-case letter.** We recommend you to use Pascal Case. You can reference labels that weren't declared before your current line, as long as every label has an unique name. **There can only be one label declaration per line.**

You can jump through labels using jump tasks, of which we'll talk about very soon.

## Arguments
Not everything is a constant. Let's say that you want to perform a certain quantum calculation in function of a number that could vary. How would you pass that value to your beautiful algorithm? Well, they're called your algorithm "arguments", and in nyancat code, you can declare them this way:

`< foo, bar >`

**Arguments must start with a lower-case letter or an underscore.** We recommend you to use Camel Case.

In contrast with labels, it is not possible to reference an argument that wasn't declared previously.

You can declare many arguments, many times, but **only once per line.** Trying to open another pair of angle brackets (`<>`) on the same line will cause your code to not work.

If you want to declare more than one argument, you must put a comma to separate them, just like the example above.

## Values and Pointers
Right now, the only plain values that you can write on your code are numeric, discrete values. You can write signed integers up to 4 bytes (`sizeof(int)` in C).

:warning: Just like a plain number, an argument reference is considered a literal numeric value.

However, differently from C, there are three types of pointers.

### Classical Pointer

A classical pointer is an unsigned integer that **represents the address of a non-reserved classical register.** That means that if you've allocated a driver that has 4 classical registers, it will only support algorithms that have pointers from "0" to "3".

You can represent a classical pointer in nyancat code like this:

`0!`

Where `0` is the classical register ID that you want to reference.

### Reserved Pointer

A reserved (classical) pointer is an unsigned integer that **represents the address of a reserved classical register.** As mentioned early, you can always know how many reserved registers will be allocated on your driver by using the constant `RESERVED_REGS` on your C code. If your driver has only 2 allocated reserved registers, trying to run an algorithm that has a reserved pointer that isn't "0" or "1" will fail.

You can represent a classical pointer in nyancat code like this:

`0%`

Where `0` is the reserved register ID that you want to reference.

:warning: Although reserved pointers have some special conditions, they are still interpreted as classical pointers.

### Quantum Pointer

A quantum pointer is an unsigned integer that **represents the address of a quantum register.** If your driver has only 3 quantum registers, an execution error will be thrown if it finds a quantum pointer outside the range "0" - "2".

You can represent a quantum pointer in nyancat code like this:

`0?`

Where `0` is the quantum register ID that you want to reference.

## Tasks
Tasks (a.k.a. Assembly's instructions) are calls to native functions that run classical or quantum operations on registers.

They're not named "instructions" because they don't talk to actual hardware here (because the hardware itself is just simulated), so I believe they're somehow different from our good old machine code instructions. Tasks can also return values and receive implicit and explicit parameters.

### Anatomy of a task
Consider the following task:

`put 1! 5`

The string `put` is what addresses what task you're calling. Each task has different parameters count and types &mdash; this one reads a classical register first and then a value &mdash; and the parameters are separated by a whitespace character. **There can only be one task call per line.**

### Return values
Many tasks return values. That means they write values into the TRR (a.k.a. Task Return Register or `0%`). This might seem a little odd at first, but it can be extremely useful when you're writing sequential tasks.

There is an exception though (like everything in life), that is the `end` task. This task is the only one that actually writes value into the ARR (a.k.a. Algorithm Return Register or `1%`). This happens because that's exactly the job of the ARR: To hold the return code of the whole algorithm.

### Parameters

As mentioned, every task will ask for at least one parameter. However, because of the quantum magic, some of the information passed to a task can be invisible to our eyes! But hold on, let's start with the simplest ones.

#### Explicit Parameters

Explicit parameters are the values we pass to a task by writing them right after the task's name. Tasks ask for specific parameters types (pointers, values, labels, etc.) in a specific order. You can check out every available task and their parameters on the [tasks table](#All-of-them) below.

##### References

You can directly pass values contained on classical registers (reserved or not) with a value reference notation, using the following syntax:

`[0!]`

Where `0` is the classical register ID that you want to reference. This can be used to pass a variable or an unknown-at-build-time value as a parameter to some task. It will work just like a hard typed numeric constant or an argument.

#### Implicit Parameters

These are the invisible parameters, but unfortunately, it has actually nothing to do with quantum magic. Some tasks are intended to have not much more than one way of being used. That's why, to avoid redundant code, some tasks have implicit parameters. They read the value on the TRR (`0%`), which is usually the return value of a previous task.

If you want a task to read a specific value as an implicit parameter, you'll have to force that behavior by using `mov` to store the desired value into the `0%` register. However, that's not a very common situation. Double check your code, mate!

### All of them

| Name | Description                                                                               | Explicit Parameters                   | Inputs from TRR | Outputs to TRR |
|------|-------------------------------------------------------------------------------------------|---------------------------------------|-----------------|----------------|
| end  | Moves the specified value to the ARR.                                     | value                     | No              | No             |
| mov  | Sets the value of the classical register referenced by the provided pointer.                        | classical_pointer value | No              | No             |
| ~~put~~  | ~~Inserts the provided value into the specified classical register.~~                         | ~~classical_pointer value~~               | ~~No~~              | ~~No~~             |
| cmp  | Compares the provided value with the classical register associated by the provided pointer and returns the result.                    | value                                 | No             | Yes            |
| jmp   | Immediately jumps to a specified label.                        | label                                 | No             | No             |
| je   | Jumps to a specified label if a previous `cmp` outputted equality.                        | label                                 | Yes             | No             |
| jne  | Jumps to a specified label if a previous `cmp` outputted inequality.                      | label                                 | Yes             | No             |
| jg   | Jumps to a specified label if a previous `cmp` outputted positive difference.             | label                                 | Yes             | No             |
| jge  | Jumps to a specified label if a previous `cmp` outputted equality or positive difference. | label                                 | Yes             | No             |
| jl   | Jumps to a specified label if a previous `cmp` outputted negative difference.             | label                                 | Yes             | No             |
| jle  | Jumps to a specified label if a previous `cmp` outputted equality or negative difference. | label                                 | Yes             | No             |
| add    | Adds the value of the classical register associated with the provided pointer to the provided value.                           | classical_pointer value                       | No              | No            |
| sub    | Subtracts the value of the classical register associated with the provided pointer to the provided value.                           | classical_pointer value                       | No              | No            |
| mul    | Multiplies the value of the classical register associated with the provided pointer to the provided value.                           | classical_pointer value                       | No              | No            |
| div    | Divides (floor) the value of the classical register associated with the provided pointer to the provided value.                   | No              | No            | No
| m    | Measures the specified quantum register and returns the result.                           | quantum_pointer                       | No              | Yes            |
| h    | Performs the Hadamard gate on the specified quantum register.                             | quantum_pointer                       | No              | No             |
| x    | Performs the Pauli's X gate on the specified quantum register.                            | quantum_pointer                       | No              | No             |
| y    | Performs the Pauli's Y gate on the specified quantum register.                            | quantum_pointer                       | No              | No             |
| z    | Performs the Pauli's Z gate on the specified quantum register.                            | quantum_pointer                       | No              | No             |

# The Hello World

Here is a simple quantum hello world made of two files:

`random_bit.nya`:
```terraform
h 0?
m 0?
end [0%]
```
---
`hello_world.c`:

```c
#include <stdio.h>
#include "../../nAR/nya/driver.h"

main() {
    driver *d = new_driver(1, 0);
    qscript r = load_script("examples/nya/random_bit", false);
    int bit = process_simple_algorithm(d, r.algorithm, false);
    printf("%s world!", bit ? "Hello" : "Goodbye");
}
```
---
:warning: This C code is oversimplified and doesn't follow the best practices. See these codes in the actual repository for a better version of them.

Let's start by analyzing the quantum code. All we do in that file is to perform a Hadamard gate on the qubit (`0?`) &mdash; a.k.a. put it in superposition &mdash; and then measure it. This will force the qubit to randomly be set to `0` or `1`. Then, we end the algorithm outputting the value on the TRR (`0%`), which is the result of the measurement.

On the C code, we allocate a driver that has 1 quantum register (we only need that one qubit) and 0 classical registers (we're not doing any classical operation with registers on our `.nya` code). Then, we load the script into a `qscript` structure, which cointains the parsed script itself and the minimum count of classical and quantum registers that must be allocated in order to that algorithm work (yes, we can input these on the driver's "constructor", and you should do this if your purpose is to run one algorithm only).

Then, we get the result of our computation by calling `process_simple_algorithm()` and store it in an `int`. You may ask: "Why is this considered a simple algorithm?". Well, you may want to call this function when your quantum algorithms don't need any arguments to be passed in (this function still passes arguments though, but they are all set to `0`). If you need to pass some information to your quantum code, you may want to use `process_algorithm()`.

And the last step is printing a different message depending on the result that we've got, which in this case, can only be `0` or `1`.

The `false` parameters on those two functions are the `echo` parameter, which is a used to activate (or not) the driver messages. As we want to display our own messages, we've set them to `false` on this example.

