--TEST--
Bug #71871: Interfaces allow final and abstract functions
--FILE--
<?php

interface test {
    abstract function test();
}

?>
--EXPECTF--
Fatal error: Interface method test::test() must not be abstract in %s on line %d
