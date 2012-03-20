(define (tak x y z)
        (if (not (< y x))
            z
          (tak (tak (- x 1) y z)
               (tak (- y 1) z x)
               (tak (- z 1) x y))))

(tak 18 9 0)


