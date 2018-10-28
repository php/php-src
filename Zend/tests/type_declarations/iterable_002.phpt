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
Fatal error: Default value for parameters with iterable type can only be an array or NULL in %s on line %d
