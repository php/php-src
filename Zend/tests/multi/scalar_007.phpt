--TEST--
Scalar type conversion: bool|string
--FILE--
<?php

function foo(bool|string $foo) {
        var_dump($foo);
}

foo(false);
foo(true);
foo("1");
foo(1);
foo(1.1);

?>
--EXPECT--
bool(false)
bool(true)
string(1) "1"
string(1) "1"
string(3) "1.1"

