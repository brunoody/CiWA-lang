
int i 0

var str "Hello and welcome to my third test!"
println "{str}"v

;; while loop

while i < 100
	i++ ;; ++ : incremental-by-1 operation
end

println "i = {i}"v

;; end while

;; for loop

for 1:10 int j =>
	;; v-string : string with variables inside
	println "value = {j}"v
end

;; end for