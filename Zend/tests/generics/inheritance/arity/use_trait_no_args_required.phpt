--TEST--
Inheritance arity: use Trait without args, trait has required type parameter, is a compile error
--FILE--
<?php
trait Holder<T> { public T $val; }
class Bad { use Holder; }
?>
--EXPECTF--
Fatal error: Too few generic type arguments to use Holder in Bad, 0 passed and exactly 1 expected in %s on line %d
