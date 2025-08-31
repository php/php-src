--TEST--
Class cannot implement BackedEnum
--FILE--
<?php

class Foo implements BackedEnum {}

?>
--EXPECTF--
Fatal error: Non-enum class Foo cannot implement interface BackedEnum in %s on line %d
