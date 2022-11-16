# CiWA: a concatenative programming language made to be simple and to compile to WebAssembly.
-- The language follows the syntatic aspects of Python and Pascal, joining with arrow-function-like syntax from JavaScript/TypeScript.
    It also features basic types such as Integer, Boolean and Float, but including a generic type variable (var). 
    It was originally written through C - thus being called CiWA - and compiled directly to wasm.
    
-- There are two basic ways of syntax writting:
    1. with arrow-function-like code after a condition/loop condition - for inline code, or
    2. with the use of the special word 'end' to start and finish the block.

# Some syntax basic examples:
## With arrow-function-like (inline code sequence):

    ### For loops:
for items => int i, var item => ...         <<<--- from the list 'items' it extracts the key 'i' and the value 'item' as a tuple

for items => var item => ...                <<<--- from the list 'items' it extracts the tuple (key, value) as well, but only keeps the value 'item' stored in memory

for -n:n => int i => ...                    <<<--- standard 'for' loop with no Array/List as anchor - from '-n' to 'n'

    ### Other:
if i > 0 => ...

while i == 0 => ...

## With special word 'end' (multiple lines of commands in the block):

    ### For loops:
for items => int i, var item                <<<--- from the list 'items' it extracts the key 'i' and the value 'item' as a tuple
    ...
end

for items => var item                       <<<--- from the list 'items' it extracts the tuple (key, value) as well, but only keeps the value 'item' stored in memory
    ...
end

for -n:n => int i                           <<<--- standard 'for' loop with no Array/List as anchor - from '-n' to 'n'
    ...
end

    ### Other:
if i > 0
    ...
end

while i == 0
    ...
end

# Basic types:
### Integer:         int[x] : where [x] equals the number of bits    (ranging from 8 to 128 bits - standard 'int' is 32 bits)

### Floating point:  float[x]: where [x] equals the number of bits   (ranging from 16 to 128 bits - standard 'float' is 32 bits)

### Boolean:         bool                                            (basic true or false)

### Data Structure:  struct                                          (class-like structure that supports methods and attributes)

### Enumerator:      enum                                            (array-like data structure that stores a tuple (key, value) data for each position)

### Array/List:      array[type]                                     (stores [0:n] elements and the counter for the elements of the given type)

### Generic:         var                                             (a generic byte array that can store any kind of value - must be casted accordingly when read - a stealthy pointer)
        PS.1: 'var' can be used to store Text values and there's no difference between char and string, or such.
        PS.2: the generic 'var', when manipulated with arithmetic operands, will be treated as an Integer.
        PS.3: the generic 'var', when manipulated with binary operands, will be treated a Byte[].
        PS.4: the generic 'var' is an actual pointer to an allocated dynamic range of memory.
        PS.5: reminder about bytes and sizes -> 2^1byte = 2^8 = 256 bytes ; 2^8bytes = 2^64 = 17.179.869.184 Gigabytes.

# Functions:
### Accepts [0:n] parameters with given types (or var, which has to be casted when read) and may or may not return a value (without the need of specification).
### Syntax examples:

func Foo var a, int b, string c             <<<--- standard expression
    ...
end

func Foo var a, int b, string c => ...      <<<--- inline expression
