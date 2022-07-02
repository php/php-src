--TEST--
Bug #71871: Interfaces allow final and abstract functions
--FILE--
<?php

interface test {
    final function test();
}

?>
--EXPECTF--
Fatal error: Interface method test::test() must not be final in %s on line %d
