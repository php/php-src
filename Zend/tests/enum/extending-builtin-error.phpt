--TEST--
GH-16315: Extending built-in enum
--FILE--
<?php

class Demo extends RoundingMode {}

?>
--EXPECTF--
Fatal error: Class Demo cannot extend enum RoundingMode in %s on line %d
