--TEST--
Accessing self::FOO in a free function
--FILE--
<?php
function test() {
    var_dump(self::FOO);
}
?>
--EXPECTF--
Fatal error: Cannot use "self" when no class scope is active in %s on line %d
