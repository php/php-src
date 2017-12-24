--TEST--
nullable scalar return type constraint (weak mode)
--FILE--
<?php

function f($p): ?scalar {
    return $p;
}

var_dump(
    f(true),
    f(false),
    f(4.2),
    f(42),
    f('str'),
    f(new class { function __toString() { return 'obj'; }}),
    f(null)
);

?>
--EXPECT--
bool(true)
bool(false)
float(4.2)
int(42)
string(3) "str"
string(3) "obj"
NULL
