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
Fatal error: baz(): Parameter #1 ($iterable) of type iterable cannot have a default value of type int in %s on line %d
