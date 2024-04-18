--TEST--
Cannot declare class, because the name is already in use
--FILE--
<?php

function test() {
    class A {}
}
test();
test();

?>
--EXPECTF--
Fatal error: Cannot declare class A, because the name is already in use (previously declared in %s:%d) in %s on line %d
