--TEST--
$GLOBALS must be passed by reference (runtime error)
--FILE--
<?php

function by_ref(&$ref) {}
try {
    by_ref($GLOBALS);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    by_ref2($GLOBALS);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
function by_ref2(&$ref) {}

?>
--EXPECT--
by_ref(): Argument #1 ($ref) could not be passed by reference
by_ref2(): Argument #1 ($ref) could not be passed by reference
