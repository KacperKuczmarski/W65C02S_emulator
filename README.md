# W65C02 emulator

Instruction-level emulator of the WDC W65C02S CPU in C++20.

- All 212 documented opcodes, including the Rockwell/WD extentions ('BBR', 'BBS', 'RMB', 'SMB', 'WAI', 'STP')
- Reserved opcodes behave as NOPs of the correct length
- Decimal mode with 65C02 flag behaviour
- IRQ, NMI, BRK and reset handling

Timing is not emulated: one step executes one whole instruction.

## Build

Requires CMake 3.20+ and a C++20 compiler.

```sh
cmake -S . -B build
cmake --build build
cmake --test-dir build
```

The test suite download GoogleTest and the functional test images.

## Usage

```sh
./build/w65c02s image.bin [--trace]
```

The image is a raw binary copied into memory at '0x0000'. Execution starts at the reset vector and runs until 'STP'.

As a library:

```cpp
w65c02s::Memory mem;
mem.load(program, 0x0200);
mem.write_word(w65c02s::vector::reset, 0x200);

w65c02s::Cpu cpu(mem);
cpu.reset();
while(!cpu.stopped()){
    cpu.step();
}
```