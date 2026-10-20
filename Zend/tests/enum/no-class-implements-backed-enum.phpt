--TEST--
Class cannot implement BackedEnum
--FILE--
<?php

class Foo implements BackedEnum {}

?>
--EXPECTF--
Fatal error: Non-enum class Foo cannot implement interface UnitEnum in %s on line %d
