--TEST--
Scalar type conversion: float|bool
--FILE--
<?php

function foo(float|bool $foo = 1) {
        var_dump($foo);
}

foo();
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
float(1)
float(1)
float(1)
float(1.1)
bool(true)
bool(false)
float(1)
float(1.1)
float(1)
float(1.1)
bool(true)

