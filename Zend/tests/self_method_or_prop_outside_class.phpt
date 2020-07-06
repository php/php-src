--TEST--
Accessing self:: properties or methods outside a class
--FILE--
<?php

$fn = function() {
    $str = "foo";
    try {
        self::${$str . "bar"};
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        unset(self::${$str . "bar"});
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        isset(self::${$str . "bar"});
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        self::{$str . "bar"}();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
};
$fn();

?>
--EXPECT--
"self" cannot be used in the global scope
"self" cannot be used in the global scope
"self" cannot be used in the global scope
"self" cannot be used in the global scope
