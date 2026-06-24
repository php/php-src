--TEST--
The ::class keyword stays case-insensitive (it is a keyword, not a member name)
--FILE--
<?php

class Foo {}

var_dump(Foo::class);
var_dump(Foo::CLASS);
var_dump(Foo::ClAsS);

?>
--EXPECT--
string(3) "Foo"
string(3) "Foo"
string(3) "Foo"
