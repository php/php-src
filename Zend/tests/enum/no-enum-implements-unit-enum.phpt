--TEST--
Enum cannot manually implement UnitEnum
--FILE--
<?php

enum Foo implements UnitEnum {}

?>
--EXPECTF--
Fatal error: Enum Foo cannot implement previously implemented interface UnitEnum in %s on line %d
