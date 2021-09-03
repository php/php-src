--TEST--
Test to ensure ::class still works
--FILE--
<?php

class Foo {}

var_dump(Foo::class);

var_dump(Foo:: class);

var_dump(Foo::	 CLASS);

var_dump(Foo::

CLASS);
?>
--EXPECT--
string(3) "Foo"
string(3) "Foo"
string(3) "Foo"
string(3) "Foo"
