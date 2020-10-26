--TEST--
Accessing self::FOO in a free function
--FILE--
<?php
function test() {
    var_dump(self::FOO);
}
?>
--EXPECTF--
Fatal error: "self" must be used only in a class scope in %s on line %d
