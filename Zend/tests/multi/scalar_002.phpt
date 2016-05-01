--TEST--
Scalar type conversion: int|string
--FILE--
<?php

function foo(int|string $foo) {
        var_dump($foo);
}

foo(1);
foo(1.0);
foo(1.1);
foo("1");
foo(true);
foo(false);
foo("1numeric");

?>
--EXPECT--
int(1)
int(1)
string(3) "1.1"
string(1) "1"
int(1)
int(0)
string(8) "1numeric"

