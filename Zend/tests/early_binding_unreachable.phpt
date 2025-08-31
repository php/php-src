--TEST--
Early bound class in otherwise unreachable code
--FILE--
<?php
class A {}
require __DIR__ . '/early_binding_unreachable.inc';
?>
--EXPECT--
object(B)#1 (0) {
}
