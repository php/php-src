--TEST--
Class cannot implement UnitEnum
--FILE--
<?php

class Foo implements UnitEnum {}

?>
--EXPECTF--
Fatal error: Non-enum class Foo cannot implement interface UnitEnum in %s on line %d
