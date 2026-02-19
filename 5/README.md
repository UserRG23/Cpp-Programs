The task is to develop a class that implements a simplified linker.

Linker is a component of a compiler. Linker is given object file(s) in its input, these are used to assemble the final executable. Each object file contains machine code compiled from a single module (usually, from a single source). The machine code is not ready to use, there may be missing references to functions from other modules. Next, object files include tables of so called exports and imports. The linker reads the object files, it connects the imports with the exports, it check the correctness (all imports covered by an export, no duplicate exports), it fills in the computed addresses, and it writes the finished machine code back to disk. The required class implements a simplification of the above tasks.

The core of an object file is the compiled machine code. It contains the processor instructions (we consider only functions in this task). The bytes that form the machine instructions are saved in one continuous block. However, some instructions may miss the addresses - the bytes that form the address contain invalid values, often zeros. The linker overwrites these bytes with valid addresses, which are created when the functions are serialized into the resulting file. The machine code in this task is just a random sequence of bytes of some length (definitely, these are not valid instructions of any existing processor).

The linker needs to know the names of the functions that are included in the object file. The table of exports is used for this purpose. In the task, the table has a simplified structure - it contains only the name of the function (symbol) and its position in the translated code (offset, counted in bytes from the beginning of the machine code block). An object file can contain multiple functions, the functions do not overlap in the machine code block. Therefore, it is easy to identify the beginning and end of a function in the translated code; a function ends where the next function begins. Note that the order of functions in the export table may not match the order of functions in the machine code block (see the figure).

The linker also needs to know the positions in the machine code where it replaces the addresses to the called functions. The import table is used for this purpose. For each imported symbol, its name, the number of occurrences and the individual positions in the machine code are listed. Again, the positions are saved as offsets in bytes, counted from the beginning of the machine code block (see the figure).

Finally, the linker is given the name of the entry point function. This is similar to the "main" function, but we are free to choose any function name. This function body, i.e., the instructions that form the function will be copied to the resulting file first, at the very beginning of the file. The "main" function may need to call other functions. These functions are easy to identify - find all entries in the imports table that refer into the "main" function. The corresponding functions are called from "main", thus must be added to the resulting file. The functions are appended after main, the particular order is not important. However, the references from the "main" function must be updated accordingly. Of course, these newly added function may also call other functions, thus the whole procedure needs to be applied repeatedly. The linker tries to keep the resulting file as short as possible. That is, it does not copy the same function repeatedly and does not insert any extra bytes between functions. Furthermore, unreachable functions are not stored into the resulting file. The order of functions in the result file is not specified, but the first function must be the function corresponding to the specified entry point ("main").

For the sake of simplicity, we assume all addresses 32 bits in size, we assume only little-endian byte order (intel), and all addresses are absolute (the position in bytes, counted from the beginning of the machine code block).

Object file structure:

> offset  size   meaning
> 0000    4B     number of entries in the exports table
> 0004    4B     number of entries in the imports table
> 0008    4B     total size of the machine code block (in bytes)
> 000C    ?      exports table, # of entries in given by the integer at offset 0
> ????    ?      imports table, # of entries in given by the integer at offset 4
> ????    ?      machine code block, total length is given by the integer at offset 8

An entry in the exports table:

> +0000   1B     function name length
> +0001   ?      function name
> +????   4B     position of the function, counted in bytes from the beginning of the machine code block

An entry in the imports table:

> +0000   1B     function name length
> +0001   ?      function name
> +????   4B     N, the # of times the function is used (called) in the machine code block
> +????   N * 4B the positions where the function is used, counted in bytes from the beginning of the machine code block

Example:
An input object file may consist the following bytes:

> 0000    03 00 00 00  02 00 00 00  30 00 00 00               // 3 exports, 2 imports, total of 48 = 0x30 bytes in the machine code block
> 
> 000C    03 66 6f 6f  08 00 00 00                            // 1. export, function foo, offset 8 in the machine code block
> 0014    02 61 61 00  00 00 00                               // 2. export, function aa, offset 0 in the machine code block
> 001B    01 58 18 00  00 00                                  // 3. export, function X, offset 24 = 0x18 in the machine code block
> 
> 0021    02 61 61 01  00 00 00 1A  00 00 00                  // 1. import, function aa is called 1x at position 0x1A
> 002C    03 66 6f 6f  02 00 00 00  27 00 00 00  09 00 00 00  // 2. import, function foo is called 2x at positions 0x27 and 0x9
> 
> 003C    01 02 03 04  05 06 07 08                            // function aa, implementation
> 0044    21 62 33 64  32 46 15 38  39 7A 34 34  12 1D 2F 48  // function foo, implementation. Bytes 62 33 64 32 will be replaced
>                                                             // by the address of foo (offset 0x9 in the imports table)
> 0054    5D 2C 47 15  2E 4F 83 A7  C5 EE 2B 47  56 61 2D 38  // function X, implementation. Bytes 47 15 2E 4F will be replaced
> 0064    3B 48 51 17  63 90 5A 3C                            // by the address of aa (offset 0x1A in the imports table),
>                                                             // bytes 38 3B 48 51 will be replaced the address of foo (offset 0x27
                                                            // in the imports table)

When passed to the linker with entry point function aa, the resulting file will be 8 bytes long. Only function aa will be copied, the implementation will remain unchanged (no function called from within aa):

> 0000    01 02 03 04  05 06 07 08

If function X is the entry point, the resulting file must contain functionsX, foo, and aa. Moreover, the addresses must be updated:

> 0000    5D 2C 18 00  00 00 83 A7  C5 EE 2B 47  56 61 2D 20  // implementation of X, bytes 18 00 00 00 form the address of
> 0010    00 00 00 17  63 90 5A 3C                            // function aa, bytes 20 00 00 00 form the address of foo
> 
> 0018    01 02 03 04  05 06 07 08                            // function aa, no modifications
> 
> 0020    21 20 00 00  00 46 15 38  39 7A 34 34  12 1D 2F 48  // function foo, bytes 20 00 00 00 form the address of foo

---

The required class CLinker has the following interface:

default constructor and destructor
    in the usual meaning
copy constructor and operator =
    not used in this assignment, may be suppressed (=delete).
addFile(objFileName)
    adds a new object file to the linker. Parameter is the name of the object file. The method may read the file, or it may just save the file name and read the file contents later. If you read the object files in this method and the reading fails, throw std::runtime_error, see below.
linkOutput ( outFileName, entryPoint )
    the method creates the output file outFileName. The second parameter is the name of the entry point function (i.e., the equivalent of "main"). The method does not return anything. Exception std::runtime_error is thrown if the linking fails (missing/duplicate symbol, I/O error, ...). The description of the exception may contain the reason of the error (this may be helpful when debugging). However, the description of the exception is not checked by the testing environment. 
