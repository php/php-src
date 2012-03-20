(define (fact n)
        (if (< n 2)
            1
          (* n (fact (- n 1)))))

(dotimes (i 10000)
  (fact 100))

