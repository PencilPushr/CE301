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
