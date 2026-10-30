--TEST--
iterable type#002 - Default values
--FILE--
<?php

function foo(iterable $iterable = null) {
    // Null should be allowed as a default value
}

function bar(iterable $iterable = []) {
    // Array should be allowed as a default value
}

function baz(iterable $iterable = 1) {
    // No other values should be allowed as defaults
}

?>
--EXPECTF--
Deprecated: foo(): Implicitly marking parameter $iterable as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d

Fatal error: Cannot use int as default value for parameter $iterable of type Traversable|array in %s on line %d
