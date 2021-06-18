--TEST--
Partial application variation no call order of destruction
--FILE--
<?php
class Foo {
    function method() {}
}
$foo = new Foo;
$foo->method(...)(...);

echo "OK";
?>
--EXPECT--
OK
