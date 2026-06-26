--TEST--
Inheritance arity: extends Pair<K, V=mixed> with 3 args is a compile error
--FILE--
<?php
class Pair<K, V = mixed> {}
class Bad extends Pair<int, string, float> {}
?>
--EXPECTF--
Fatal error: Too many generic type arguments to extends Pair in Bad, 3 passed and at most 2 expected in %s on line %d
