--TEST--
print_r(Object)
--FILE--
<?php
class Foo {}
$foo = new Foo;
print_r($foo);
?>
--EXPECT--
Foo Object
(
)
