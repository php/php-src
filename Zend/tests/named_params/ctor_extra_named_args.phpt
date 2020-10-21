--TEST--
Passing unknown named args to a non-existing ctor
--FILE--
<?php

try {
    new stdClass(x: "nope");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Unknown named parameter $x
