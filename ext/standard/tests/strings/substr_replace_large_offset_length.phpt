--TEST--
Test substr_replace() function : large offset & length
--FILE--
<?php

echo "*** Very large offset ***\n";
try {
    var_dump(substr_replace('hello', 'X', PHP_INT_MAX));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(substr_replace('hello', 'X', PHP_INT_MIN));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "*** Very large length ***\n";
try {
    var_dump(substr_replace('hello', 'X', 0, PHP_INT_MAX));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(substr_replace('hello', 'X', 0, PHP_INT_MIN));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Very large offset ***
string(6) "helloX"
string(1) "X"
*** Very large length ***
string(1) "X"
string(6) "Xhello"
