--TEST--
Bug #44827 (Class error when trying to access :: as constant)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest Munich 2009
--FILE--
<?php
try {
    define('::', true);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    constant('::');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
ValueError: define(): Argument #1 ($constant_name) cannot be a class constant
Error: Class "" not found
