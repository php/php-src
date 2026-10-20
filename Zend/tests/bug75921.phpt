--TEST--
Bug #75921: Inconsistent error when creating stdObject from empty variable
--FILE--
<?php

try {
    $null->a = 42;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($null);
unset($null);

try {
    $null->a['hello'] = 42;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($null);
unset($null);

try {
    $null->a->b = 42;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($null);
unset($null);

try {
    $null->a['hello']->b = 42;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($null);
unset($null);

try {
    $null->a->b['hello'] = 42;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($null);
unset($null);

?>
--EXPECTF--
Error: Attempt to assign property "a" on null

Warning: Undefined variable $null in %s on line %d
NULL
Error: Attempt to modify property "a" on null

Warning: Undefined variable $null in %s on line %d
NULL
Error: Attempt to modify property "a" on null

Warning: Undefined variable $null in %s on line %d
NULL
Error: Attempt to modify property "a" on null

Warning: Undefined variable $null in %s on line %d
NULL
Error: Attempt to modify property "a" on null

Warning: Undefined variable $null in %s on line %d
NULL
