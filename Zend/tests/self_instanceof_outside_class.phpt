--TEST--
instanceof self outside a class
--FILE--
<?php

$fn = function() {
    try {
        new stdClass instanceof self;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
};
$fn();

?>
--EXPECT--
Cannot access "self" when no class scope is active
