# fe6-wizardry

This is wizardry (colloquial name for a collection of code patches, or "ASM hacks") for fe6.

## Building

1. If you didn't clone this repository recursively, run this command:
   ```
   git submodule update --init --recursive
   ```

2. Setup and build the bundled fe6 decomp (tools/fe6), see its own docs.

3. You need to have python 3 installed and in your path. On Debian/Ubuntu (default under WSL2), run this command:
   ```
   sudo apt install python3 python-is-python3
   ```

4. You need to have an arm-none-eabi toolchain (including gcc) installed. On Debian/Ubuntu (default under WSL2), run this command:
   ```
   sudo apt install gcc-arm-none-eabi
   ```

5. You need to get [EventAssembler][EventAssembler] into tools/EventAssembler.

6. You should be ready to go. type `make` to make the test ROM and its included wizardry.

[EventAssembler]: https://github.com/StanHash/EventAssembler
