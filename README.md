# CE301

As I've been removed from the Uni's database, I could not retrieve the latest version of the project, however I do not wish to lose this.


## How to run (Windows only)

Upon running (provided it does so succesfully) you will be prompted to select a executable file.
The selected filed must below a page in size (4096 bytes/4kb), and of the x86 architecture.
This likely requires shrinking the executable, such as:
 - not linking the standard library,
 - compiling static information embedded into the executable
 - ... etc.

Should the file fufill the size requirement, it will be disassembled from machine code back into x86 assembly.

## Libraries
- Zydis
- Imgui
- Imgui_club

## Program outline
1. Creation of valid code, waiting to be compiled.
2. Code is compiled via compiler (Black box) and generates an appropriate executable.
3. The sections (main hex/assembly) are extracted from the .exe.
4. Are then parsed and loaded into the emulator appropriately.
5. Emulator deciphers the symbols/hex and disassembles the hex back into assembly. Shown via the emulator GUI (infront) and IDA pro 7.5 (behind).
<br/><br/>
![alt text](https://github.com/PencilPushr/CE301/blob/main/openday.png?raw=true)
<br/><br/>
The user can compare and measure against another program the contents of sections, length of asm instructions.
The emulator also allows for single stepping to view execution process.
