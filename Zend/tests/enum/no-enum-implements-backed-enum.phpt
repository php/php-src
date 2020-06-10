--TEST--
Enum cannot manually implement BackedEnum
--FILE--
<?php

enum Foo: int implements BackedEnum {}

?>
--EXPECTF--
Fatal error: Class Foo cannot implement previously implemented interface BackedEnum in %s on line %d
