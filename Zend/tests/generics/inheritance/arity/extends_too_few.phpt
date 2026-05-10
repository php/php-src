--TEST--
Inheritance arity: extends with too few type arguments is a compile error
--FILE--
<?php
class Pair<K, V> {}
class Bad extends Pair<int> {}
?>
--EXPECTF--
Fatal error: Too few generic type arguments to extends Pair in Bad, 1 passed and exactly 2 expected in %s on line %d
