--TEST--
Accessing self:: properties or methods outside a class
--FILE--
<?php

$fn = function() {
    $str = "foo";
    try {
        self::${$str . "bar"};
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        unset(self::${$str . "bar"});
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        isset(self::${$str . "bar"});
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        self::{$str . "bar"}();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
};
$fn();

?>
--EXPECT--
Error: Cannot access "self" when no class scope is active
Error: Cannot access "self" when no class scope is active
Error: Cannot access "self" when no class scope is active
Error: Cannot access "self" when no class scope is active
