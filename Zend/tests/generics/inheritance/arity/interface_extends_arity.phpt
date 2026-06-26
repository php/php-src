--TEST--
Inheritance arity: interface extends another interface with wrong arity is a compile error
--FILE--
<?php
interface J<T> {}
interface IExt extends J<int, string> {}
?>
--EXPECTF--
Fatal error: Too many generic type arguments to extends J in IExt, 2 passed and exactly 1 expected in %s on line %d
