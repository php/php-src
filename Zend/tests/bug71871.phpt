--TEST--
Bug #71871: Interfaces allow final and abstract functions
--FILE--
<?php

interface test {
    final function test();
}

?>
--EXPECTF--
Fatal error: Access type for interface method test::test() must be omitted in %s on line %d
