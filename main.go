package main

import (
	"fmt"
)

func fibonacci(n int) int {
	if n <= 1 {
		return n
	}
	return fibonacci(n-1) + fibonacci(n-2)
}

func main() {
	//start := time.Now()
	result := fibonacci(44)
	//elapsed := time.Since(start)

	fmt.Printf("Fibonacci(44) = %d\n", result)
	//fmt.Printf("Time taken: %s\n", elapsed)
}