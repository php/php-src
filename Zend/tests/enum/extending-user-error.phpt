--TEST--
Error message when extending enums (GH-16315) (extending userland enum)
--FILE--
<?php

enum MyEnum {}

class Demo extends MyEnum {}

?>
--EXPECTF--
Fatal error: Class Demo cannot extend enum MyEnum in %s on line 5
