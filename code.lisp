(define list-length (lambda (ls)
	(if (null? ls)
		0
		(+ (list-length (cdr ls)) 1)
	)
))

(define make-list (lambda (count)
	(if (< count 0)
		()
		(cons count (make-list (- count 1)))
	)
))

(define reverse-helper (lambda (ls output)
	(if (null? ls)
		output
		(reverse-helper (cdr ls) (cons (car ls) output))
	)
))

(define reverse (lambda (ls)
	(reverse-helper ls ())
))

(list-length (list #f list-length print 7 4 5))
(make-list 20)
(reverse (list 1 2 3))
