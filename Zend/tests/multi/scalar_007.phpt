--TEST--
Scalar type conversion: bool|string
--FILE--
<?php

function foo(bool|string $foo = true) {
        var_dump($foo);
}

foo();
foo(false);
foo(true);
foo("1");
foo(1);
foo(1.1);

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
string(1) "1"
string(1) "1"
string(3) "1.1"

