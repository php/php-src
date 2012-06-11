--TEST--
Yield cannot be used in normal (non-generator) functions
--FILE--
<?php

function foo() {
    yield "Test";
}

?>
--EXPECTF--
Fatal error: The "yield" expression can only be used inside a generator function in %s on line %d
