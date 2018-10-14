(begin
  (define a (+ 1 I))
  (define b (set-property "note" "complex number" a))
  (get-property "note" b)
)
