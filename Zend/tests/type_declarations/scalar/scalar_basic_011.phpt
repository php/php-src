--TEST--
nullable scalar return type constraint (strict mode)
--FILE--
<?php

declare(strict_types = 1);

function f($p): ?scalar {
    return $p;
}

var_dump(
    f(true),
    f(false),
    f(4.2),
    f(42),
    f('str'),
    f(null)
);

?>
--EXPECT--
bool(true)
bool(false)
float(4.2)
int(42)
string(3) "str"
NULL
