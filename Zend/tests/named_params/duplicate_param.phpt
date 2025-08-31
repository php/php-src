--TEST--
Duplicate param
--FILE--
<?php

function test($a) {}

try {
    test(a: 1, a: 2);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    test(1, a: 2);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Named parameter $a overwrites previous argument
Named parameter $a overwrites previous argument
