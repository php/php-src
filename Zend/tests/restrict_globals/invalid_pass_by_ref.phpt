--TEST--
$GLOBALS must be passed by reference (runtime error)
--FILE--
<?php

function by_ref(&$ref) {}
try {
    by_ref($GLOBALS);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    by_ref2($GLOBALS);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
function by_ref2(&$ref) {}

?>
--EXPECT--
Error: by_ref(): Argument #1 ($ref) could not be passed by reference
Error: by_ref2(): Argument #1 ($ref) could not be passed by reference
