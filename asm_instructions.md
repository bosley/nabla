# This will be expanded as things move along, 

## Registers

Prefixed with 'r' there are 16 8-byte registers [ r0, r15 ]
These registers are used to read/write any data to/from.
The addresses for the registers are [0b, 15b]

[128 bytes overhead]

## Addresses

An address is comprised of 4 bytes, giving us 2^32 possible addresses.

## Comments

Everything after a stray ';' on a line will be treated as a comment

## Read-only system registers

Prefixed with 'sys' there are 2 system registers [sys0, sys1]
These registers are read-only to the application. They are used by the system for storing return addresses
and performing system functions.
Each register is 4 bytes.

[8 bytes in total]

## Functions

Functions begin with a '<' followed by a name and a ':'. Functions are each given their own operational stack. This allows for the separation of functional memory. Standard function's stack will be emptied upon the return from that function. Every program function can access the following: its own stack, the global stack, and the system registers.

To access a function, a 'call' must occur, you can not jump to a function. Further, you can not jump to a label in another function. Jumping is localized to the function that is currently being operated in.

### Pseudo Ops
|  Directive 	|   Argument	|   Description                                 |
|---	        |---	        |---	                                        |
|   .file	    |  "file name"  |   emit filename FILE_LOCAL to symbol table	|
|   .init	    |  entry_label 	|   emit entry_label APP_ENTRY to symbol table 	|
|   .string	    |  "string"     |   emit "string" ID to string table	        |
|   .int	    |     42        |   emit 42 ID to integer table	                |
|   .double	    |   3.14 	    |   emit 3.14 ID to double table            	|
|   .include	|  "file.asm" 	|   add "file,asm" to source                   	|

### In-place numerical values

For arithmatic operations a drop-in numerical value can be given to an instruction. This drop-in is the numerical value prefixed
with a '$'. These numerical drop-ins are unsigned and encoded directly into the instruction. The valid range for an in-place value
is the maximum number able to be stored by 2 bytes (2^16) 

### Directive references

Constant ints strings and doubles defined by their corresponding directive will yeild their address when placed within an instruction
by prefixing them with a '&'. Since they are addresses, they must be loaded into a register prior to performing an arithmatic operation.
This can be done using the 'ldb' command, which will drop their value into a register.

## Instructions
Abbreviations : 
| Abbreviation | Meaning                                |
|---           |---                                     |
|      r       | register                               |
|     sys      | system register                        |
|     *n       | in-place numerical value               |
|     *i       | constant int ref    (address)          |
|     *s       | constant string ref (address)          |
|     *d       | constant double ref (address)          |
|     sp       | stack pointer   (ls, gs)               |
|     *sp      | stack pointer offset  ($N(ls), $N(gs)) |

## Artihmatic Instructions
| Instruction     | Arg1      | Arg2          | Arg3         | Description                                  |
|---              |---        |---            |---           |---                                           |
|     add         |        r  |    r , *n     |   r , *n     |  Add Arg2 and Arg3, Store in Arg1            |
|     sub         |        r  |    r , *n     |   r , *n     |  Sub Arg3 from Arg3, Store in Arg1           |
|     mul         |        r  |    r , *n     |   r , *n     |  Mul Arg2 and Arg3, Store in Arg1            |
|     div         |        r  |    r , *n     |   r , *n     |  Div Arg3 by  Arg2, Store in Arg1            |
|     add.d       |        r  |      r        |     r        |  Add (double) Arg2 and Arg3, Store in Arg1   |
|     sub.d       |        r  |      r        |     r        |  Sub (double) Arg3 from Arg3, Store in Arg1  |
|     mul.d       |        r  |      r        |     r        |  Mul (double) Arg2 and Arg3, Store in Arg1   |
|     div.d       |        r  |      r        |     r        |  Div (double) Arg3 by  Arg2, Store    in Arg1   |
   
Arithmatic instructions that specify 'd' assumes that the values being operated on are double-precision floating point
numbers, if the value in a given 'd' register is not a floating point, the behaviour is undefined.

## Branch Instructions

| Instruction   | Arg1           | Arg2             | Arg3               | Description                               |
|---            |---             |---               |---                 |---                                        |
|      bgt      |  r             |   r              |   label            | Branch to Arg3 if Arg1 > Arg2             |
|      bgte     |  r             |   r              |   label            | Branch to Arg3 if Arg1 >= Arg2            |
|      blt      |  r             |   r              |   label            | Branch to Arg3 if Arg1 <= Arg2            |
|      blte     |  r             |   r              |   label            | Branch to Arg3 if Arg1 <= Arg2            |
|      beq      |  r             |   r              |   label            | Branch to Arg3 if Arg1 == Arg2            |
|      bne      |  r             |   r              |   label            | Branch to Arg3 if Arg1 != Arg2            |
|      bgt.d    |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 > Arg2    |
|      bgte.d   |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 >= Arg2   |
|      blt.d    |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 <= Arg2   |
|      blte.d   |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 <= Arg2   |
|      beq.d    |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 == Arg2   |
|      bne.d    |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 != Arg2   |

Branch instructions assume that the conditional values stored in registers are integer values unless 'd' is specified. 
If 'd' is specified and the value in a given register is not a floating point, the behaviour is undefined.

Currently branches must branch to existing labels, that is, labels that come before them within the function.

## Loading / Storing Instructions
|  Instruction     |  Arg1     |  Arg2                   |  Description                                 |
|---               |---        |---                      |---                                           |
|      mov         |    r      |   r                     |  Move data from Arg2 into Arg1               |
|      lda         |    r      |   *i, *s, *d, *sp       |  Load address of Arg2 into Arg1              |
|      ldb         |    r      |   r, *i, *s, *sp, *d    |  Load data from Arg2 (address) into Arg1     |
|      stb         |   *sp     |   r                     |  Store Arg2 data at Arg1 (arg1=addr)         |
|      push        |    sp     |   r                     |  Data from Arg2 into Arg1                    |
|      pop         |    sp     |   r                     |  Data from Arg2 into Arg1                    |


**DEVELOPMENT NOTE:** Consider adding in-place int and double placement in PUSH, as-well-as *i, *s, and *d

*Note:* Moves destroy data in source. It will 0-out the data.
*Note:* ldb assumes data from Arg2 represents an address, and will attempt to pull data from whatever address it assumes. We can leverage a dReg to load string data if we want to, but performing arithmatic operations on data representing a string will result in undefined behaviour.
*Note:* A mix of rRegs and dRegs can be used for mov and ldb, but it is important to note that since rRegs store only 4 bytes,
if you move rReg data to dReg data and attempt an arithmatic operation that is meant for dRegs undefined behaviour will result. 

## Jump / Call

| Instruction | Arg1     | Description                                       |
|---          |---       |---                                                |
| jmp         | label    | Jump to label                                     |
| call        | function | Call function - return address stored sys stack   |
| ret         |          | Return to the address stored on top of sys stack  |


## Exit

| Instruction | Description                                   |
|---          |---                                            |
| exit        | Quit execution of program                     |


## Functions / Labels

| Instruction    | Description                          |
|---             |---                                   |
| <funcName:     | Create function 'funcName'           |
| >              | End of function declaration          |
| labelName:     | Create label 'labelName'             |



## Instruction Data

Each full instruction is 8 bytes. 

### Arithmatic operations

[Byte 1]
The first 6 bits represent the specific instruction (add / mul/ etc)
The remaining 2 bits of the first byte indicate what the remaining bytes represent.

The indication bits are as follows:
00 - Byte 3 will be a register, Byte 4 will be a register
01 - Byte 3 will be a register, Byte 4/5 will be a 2-byte integer
10 - Byte 3/4 will be a 2-byte integer, Byte 5 will be a register
11 - Byte 3/4 will be a 2-byte integer, Byte 5/6 will be a 2-byte integer

[Byte 2]
The second byte is the destination register

Unaccounted bytes will be unused, and marked as '1'

**Note:** Since drop-ins are not allowed on double-based arithmatic operations, the id bits listed above
and displayed below are not applicable. Instead, double-based arithmatic operations will default to the 
'00' case listed below.

Here is an example of the bit layout given an arithmatic operation. Note: All but ID here are filled
with '1' just for the sake of demonstration

    Case 00:
    INS    ID   REGISTER    REGISTER    REGISTER  [ ----------------- UNUSED -------------------]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    Case 01:
    INS    ID   REGISTER    REGISTER    [ ---- INTEGER ---- ]   [ -------- UNUSED --------------]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    Case 10:
    INS    ID   REGISTER    [ ---- INTEGER ---- ]   REGISTER    [ -------- UNUSED --------------]
    111111 10 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    Case 10:
    INS    ID   REGISTER    [ ---- INTEGER ---- ]   [ ---- INTEGER ---- ]   [ ---- UNUSED ---- ]
    111111 11 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111
    
### Branch operations

[Byte 1]
The first 6 bits represent the specific instruction (bge / blt / etc)
The following 2 bits will be set to 00, unused, but also represents that 2 registers are being used.

[Byte 2-3]
The following 2 bytes, are the registers that are used for the branch comparison.

[Byte 4-7]
The following 4 bytes will be the address to branch to.

[Byte 8]
The last byte is unused 

Here is an example of a bit layout for a branch operation

    INS    ID   REGISTER    REGISTER    [ --------------   ADDRESS  ----------------]   [ UNUSED ] 
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

### Load / Store operations

The first 6 bytes represent the specific instruction (mov / movd / etc)

Since not all load / store operations are the same their bit fields differ slightly by specific instruction.
For all instructions except ldw/ldwd the ID bits don't matter. 

**mov**

    INS    ID   REGISTER    REGISTER    [ ----------------------- UNUSED ---------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**lda**

    INS    ID   REGISTER    [ ---------------   ADDRESS  ---------------]   [ ----- UNUSED ---- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**stb**

    INS    ID   [ ---------------   ADDRESS  ---------------]   REGISTER    [ ---- UNUSED ----- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**ldb**

Id bits:
00 - Byte 3 is a register (presumably with an address in it)

    INS    ID   REGISTER    REGISTER    [ ----------------------- UNUSED ---------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

01 - Byte 3-6 is an address encoded into the instruction

    INS    ID   REGISTER    [ ---------------   ADDRESS  ---------------]   [ ---- UNUSED ----- ]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**push**

    INS    ID      STACK     REGISTER   [ ------------------- UNUSED -------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**pop**

    INS    ID   REGISTER      STACK     [ --------------------   UNUSED  ---------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111


### Jump / return operation

**jump**

The jump operation is straight forward. The only data in the jump is the address to jump to.

    INS    ID   [ ---------------   ADDRESS  --------------- ]  [ ---------- UNUSED ----------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**call**

Upon executing the call instruction, the address immediatly after call's address will be stored in
the system stack. 

    INS    ID   [ ---------------   ADDRESS  --------------- ]  [ ---------- UNUSED ----------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**return**

The return address will be on the top of the system stack. Executing return will return to whatever
address is on the top of the system stack, and then pop it. 

    INS    ID  [ ----------------------------------- UNUSED ----------------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

Note: Implicit returns happen at the bottom of a function. If the bottom of the function is reached, 
a return will occur.


### Functions / Labels

Function start / end are special instructions that are scanned for upon initial program load
to tell the VM how to structure its self. 

**function start**

    INS    ID   [ --------------------- NUM INSTRUCTIONS IN FUNC ------------------------------ ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**function end**

    INS    ID   [ ---------------------------------- UNUSED ----------------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

## Constants

Constant creation instructions dont follow standard instruction layout which is why they were saved for last.

**.int**

id bits: 00 - 8-bit  int [1 byte]
id bits: 01 - 16-bit int [2 bytes]
id bits: 10 - 32-bit int [4 bytes]
id bits: 11 - 64-bit int [8 bytes]

    INS    ID   [ --- INTEGER BYTES ---- ]
    111111 00 | 1111 1111 .... | 1111 1111

**.string**

id bits ignored

The current maximum is 256 bytes for a string

    INS    ID    BYTES      [ ------- STR BYTES ------ ]
    111111 00 | 1111 1111 | 1111 1111 | .... | 1111 1111

**.double**

id bits ignored 

    INS    ID   [ ------------------------------- DOUBLE DATA -------------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111


## The stack

In this system, there are multiple stacks, but only 2 that can be accessed at any given time programatically. The functional stack, and the global stack. Each slot of each stack is 8 bytes. 

### Functional stack

A stack accessed by 'ls' for 'local stack' that accesses the stack preserved for the current function. Once the system leaves the function, the local stack is blown away. 

### Global stack

A stack accessed by 'gs' for 'global stack' that accesses the stack used across the entire program. Great care should be taken when using this stack, as there is no garbage collection in the scope of what I care to do here.

### System stack

Not able to be accessed by software directly. The system stack is pushed and popped by calls and returns.