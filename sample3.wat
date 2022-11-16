(module
    (import "console" "log" (func $println (param i32 i32)))

    (memory 1 1024)

    (func (export "main") (local $i i32) (local $j i32)

        i32.const 0
        local.set $i

        ;; while loop
        (block $while_0
            (loop
                ;; test if $i is less than 100 and breaks the loop if true
                local.get $i
                i32.const 100
                i32.lt_s
                br_if 1
                
                i32.const 1
                local.get $i
                i32.add
                local.set $i

                br 0 ;; send back to loop-start
            )
        )

        ;; for loop
    )
)