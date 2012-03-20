(define (ack m n)
  (cond ((zero? m) (+ n 1))
	((zero? n) (ack (- m 1) 1))
	(else      (ack (- m 1) (ack m (- n 1))))))

(ack 3 9)

