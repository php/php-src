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
    echo $e->getMessage(), "\n";
}

try {
    constant('::');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
define(): Argument #1 ($constant_name) cannot be a class constant

Fatal error: Class "" not found in %s on line %d
