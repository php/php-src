--TEST--
instanceof self outside a class
--FILE--
<?php

$fn = function() {
    try {
        new stdClass instanceof self;
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
};
$fn();

?>
--EXPECT--
Error: Cannot access "self" when no class scope is active
