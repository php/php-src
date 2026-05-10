--TEST--
Recursive bounds: forward reference in a default is rejected
--FILE--
<?php
class Foo {}
class C<T: Foo = U, U: Foo = T> {}
?>
--EXPECTF--
Fatal error: Type parameter U referenced before declaration in %s on line %d
