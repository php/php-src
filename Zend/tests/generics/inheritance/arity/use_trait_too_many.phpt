--TEST--
Inheritance arity: use trait with too many type arguments is a compile error
--FILE--
<?php
trait Holder<T> { public T $v; }
class Bad { use Holder<int, string>; }
?>
--EXPECTF--
Fatal error: Too many generic type arguments to use Holder in Bad, 2 passed and exactly 1 expected in %s on line %d
