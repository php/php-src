--TEST--
Accessing self::FOO in a free function
--FILE--
<?php
function test() {
    var_dump(self::FOO);
}
?>
--EXPECTF--
Fatal error: "self" cannot be used in the global scope in %s on line %d
