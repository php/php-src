--TEST--
Errors: duplicate type parameter inside a method's own list
--FILE--
<?php
class C { public function f<T, T>(): void {} }
?>
--EXPECTF--
Fatal error: Cannot redeclare type parameter T in %s on line %d
