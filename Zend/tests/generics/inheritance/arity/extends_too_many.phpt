--TEST--
Inheritance arity: extends with too many type arguments is a compile error
--FILE--
<?php
class Box<T> {}
class Bad extends Box<int, string> {}
?>
--EXPECTF--
Fatal error: Too many generic type arguments to extends Box in Bad, 2 passed and exactly 1 expected in %s on line %d
