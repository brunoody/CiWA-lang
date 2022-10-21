# WebAssembly Textual basics & code examples

# basic codes
(module)    <= root node of the code tree
(func)      <= function declaration
(param x)   <= parameter declaration for a determined function
                where x equals to the type
(result y)  <= result of the function ; where y equals to the type
(local z)   <= parameter that can be read and written by the function internally
                substitutes the result
$name       <= parameters can be named with the $tag-name before setting its type;
                $tag-name can also name functions
call        <= calls a function by its $tag-name
import      <= to import a know module
export      <= exports module with <string> name
global      <= a global used variable
                in imports: (global $g (import "js" "global") (mut i32))
                'get' use: (global.get $g)
memory      <= large array of bytes that can grow over time
                (generic for string, structs and byte[])
(start)     <= starts and runs the module

# basic types
-- Integer: i32, i64 (32-64bit)
-- Float:   f32, f64 (32-64bit)

# sample 1
(module
    (func $add (param $p1 i32) (param $p2 i32) (result i32)
        local.get $p1
        local.get $p2
        i32.add
    )
    (export "add" (func $add))
)

# sample 2
(module
    (func $copy (param $p1 i64) (local $loc i64)
        local.get $p1
        local.set $loc
    )
    (export "copy" (func $copy))
)

# sample 3
-- TBD : working with memory

# sample 4
-- TBD : working with memory and files (as globals)

# sample 5
-- TBD : working with tables

# code order step-by-step
1- open module code                 -> (module ...)
2- declare function with its name   -> (func $name ...)
3- embed parameters with names      -> (param|local $pname)
4- codes inside the tree structure  -> ...
5- export function-exported-name    -> (export "name" (func $name))

# Questions:
a. A module can be related directly to C-structs, right?
b. Which paramater types can be accepted, so you can have any type of byte[] translated into WAsm code?
c. How many exports can we have inline and/or inside a unique module, thus having a class-like final structure?
