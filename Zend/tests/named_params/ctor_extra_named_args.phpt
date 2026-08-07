--TEST--
Passing unknown named args to a non-existing ctor
--FILE--
<?php

class Test {}

try {
    new stdClass(x: "nope");
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new Test(x: "nope");
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Unknown named parameter $x
Error: Unknown named parameter $x
