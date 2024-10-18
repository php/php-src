--TEST--
Error message when extending enums (GH-16315) (extending built in enum)
--FILE--
<?php

class Demo extends RoundingMode {}

?>
--EXPECTF--
Fatal error: Class Demo cannot extend enum RoundingMode in %s on line 3
