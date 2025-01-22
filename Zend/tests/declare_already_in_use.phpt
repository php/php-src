--TEST--
Cannot redeclare class
--FILE--
<?php

function test() {
    class A {}
}
test();
test();

?>
--EXPECTF--
Fatal error: Cannot redeclare class A (previously declared in %s:%d) in %s on line %d
