--TEST--
Unknown named parameter
--FILE--
<?php

function test($a) {
}

function test2(...$a) {
}

try {
    test(b: 42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    test2(a: 42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Unknown named parameter $b
