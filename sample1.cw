;; first simple example

int i 0 var a 0 int b 0

if i == 0 ; if it's equal to 0 then sets 'a'
    a 1
elsif i > 0 ; else if it's greater then sets 'b'
    b 1
end

if a == 1
    a "a is 1" ; var int to generic reattribution
    println a
elsif b == 1
    println "b is 1"
end

;; end of test #1