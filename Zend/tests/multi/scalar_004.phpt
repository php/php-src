--TEST--
Scalar type conversion: int|float|bool
--FILE--
<?php

function foo(int|float|bool $foo) {
        var_dump($foo);
}

foo(1);
foo(1.0);
foo(1.1);
foo(true);
foo(false);
foo("1");
foo("1.1");
foo("1numeric");
foo("1.1numeric");
foo("invalid");

?>
--EXPECTF--
int(1)
float(1)
float(1.1)
bool(true)
bool(false)
int(1)
float(1.1)
int(1)
float(1.1)
bool(true)

