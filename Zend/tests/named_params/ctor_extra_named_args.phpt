--TEST--
Passing unknown named args to a non-existing ctor
--FILE--
<?php

class Test {}

try {
    new stdClass(x: "nope");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    new Test(x: "nope");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Unknown named parameter $x
Unknown named parameter $x
