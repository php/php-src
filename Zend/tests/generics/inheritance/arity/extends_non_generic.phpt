--TEST--
Inheritance arity: extends a non-generic class with type arguments is a compile error
--FILE--
<?php
class Plain {}
class Bad extends Plain<int> {}
?>
--EXPECTF--
Fatal error: Too many generic type arguments to extends Plain in Bad, 1 passed and exactly 0 expected in %s on line %d
