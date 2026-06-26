--TEST--
static fn() {} is disallowed for scope functions
--FILE--
<?php
function test() {
    $fn = static fn() { return 1; };
}
?>
--EXPECTF--
Fatal error: Scope functions cannot be static in %s on line %d
