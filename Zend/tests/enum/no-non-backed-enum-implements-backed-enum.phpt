--TEST--
Non-backed enum cannot implement BackedEnum
--FILE--
<?php

enum Foo implements BackedEnum {}

?>
--EXPECTF--
Fatal error: Non-backed enum Foo cannot implement interface BackedEnum in %s on line %d
