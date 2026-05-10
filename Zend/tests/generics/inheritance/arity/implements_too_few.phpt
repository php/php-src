--TEST--
Inheritance arity: implements with too few type arguments is a compile error
--FILE--
<?php
interface Map<K, V> {}
class Bad implements Map<int> {}
?>
--EXPECTF--
Fatal error: Too few generic type arguments to implements Map in Bad, 1 passed and exactly 2 expected in %s on line %d
