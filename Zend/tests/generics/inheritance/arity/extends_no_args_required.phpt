--TEST--
Inheritance arity: extends without args, parent has required type parameter, is a compile error
--FILE--
<?php
class Box<T> {}
class Bad extends Box {}
?>
--EXPECTF--
Fatal error: Too few generic type arguments to extends Box in Bad, 0 passed and exactly 1 expected in %s on line %d
