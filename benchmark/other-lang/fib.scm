(define (fib n)
        (if (< n 3)
            1
          (+ (fib (- n 1)) (fib (- n 2)))))

(fib 34)

