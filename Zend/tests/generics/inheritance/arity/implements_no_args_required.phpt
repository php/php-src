--TEST--
Inheritance arity: implements without args, interface has required type parameter, is a compile error
--FILE--
<?php
interface I<K, V> {}
class Bad implements I {}
?>
--EXPECTF--
Fatal error: Too few generic type arguments to implements I in Bad, 0 passed and exactly 2 expected in %s on line %d
