--TEST--
Optional param before required should be treated as required for named args as well
--FILE--
<?php

function test($a = 1, $b) {
}
try {
    test(b: 2);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Deprecated: Optional parameter $a declared before required parameter $b is implicitly treated as a required parameter in %s on line %d
test(): Argument #1 ($a) not passed
