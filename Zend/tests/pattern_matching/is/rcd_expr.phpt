--TEST--
Refcoutned expression
--FILE--
<?php

class Foo {}

var_dump(new Foo() is Foo);
var_dump(new Foo() is Bar);
var_dump(match (new Foo()) {
    is Bar => throw new Exception('Unreachable'),
    is Foo => 'foo',
});

?>
--EXPECT--
bool(true)
bool(false)
string(3) "foo"
