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
Fatal error: class A cannot be declared, because the name is already in use in %s on line %d
