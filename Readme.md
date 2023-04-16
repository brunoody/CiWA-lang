# CiWA: a concatenative programming language made to be simple and to compile to WebAssembly.
-- The language follows the syntatic aspects of Python and Pascal, joining with arrow-function-like syntax from JavaScript/TypeScript.
    The arrow-function-like syntax is used to maintain a logical sequence to the code in order to be able to be a concatenative language from start to finish.
    - Disclaimer: take care, but this syntax allows the developer to make an entire code all inline!
    It also features basic types such as Integer, Boolean and Float, but including a generic type variable (var). 
    It was originally written through C - thus being called CiWA - and aimed to compile directly to wat/wasm.
    
-- The basic syntax is to initiate a block with the desired keyword and close it with the keyword 'end'.
-- The arrow keyword '=>' is used to define For-loops iterating through an array-list.

# Some syntax basic examples:
### For loops:

for items int i, var item =>             <<<--- from the list 'items' it extracts the key 'i' and the value 'item' as a tuple
    ...
end

for items var item =>                    <<<--- from the list 'items' it extracts the tuple (key, value) as well, but only keeps the value 'item' stored in memory
    ...
end

for -n:n int i =>                        <<<--- standard 'for' loop with no Array/List as anchor - from '-n' to 'n', both inclusive
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
### Integer:         int[x] : where [x] equals the number of bits    (ranging from 8 to 128 bits - standard 'int' is 64 bits)

### Floating point:  float[x]: where [x] equals the number of bits   (ranging from 16 to 128 bits - standard 'float' is 64 bits)

### Boolean:         bool                                            (basic true or false)

### Data Structure:  struct                                          (class-like structure that supports methods and attributes)

### Dictionary:      dict                                            (array-like data structure that stores a tuple (key, value) data for each position)

### Array/List:      array[type]                                     (stores [0:n] elements and the counter for the elements of the given type)

### Generic:         var                                             (a generic byte array that can store any kind of value - must be casted accordingly when read - a stealthy pointer)
        PS.1: 'var' can be used to store Text values and there's no difference between nor specification for char and string, or such.
        PS.2: the generic 'var', when manipulated with arithmetic operands, will be treated as an Integer (and will be limited to the 128bit range).
        PS.3: the generic 'var', when manipulated with binary operands, will be treated a Byte[] (which is the true meaning of the 'var' type).
        PS.4: the generic 'var' is an actual pointer to an allocated dynamic range of memory, but transparent to the developers.
        PS.5: reminder about bytes and sizes -> 2^1byte = 2^8 = 256 bytes ; 2^8bytes = 2^64 = 17.179.869.184 Gigabytes.

# Functions:
## Accepts [0:n] parameters with given types (or var, which has to be casted when read) and may or may not return a value (without the need of specification).
### Syntax examples:

func Foo var a, int b, string c =>
    ...
end
