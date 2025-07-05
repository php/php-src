--TEST--
#[\Deprecated]: Basic trait deprecation
--FILE--
<?php

#[\Deprecated]
trait DemoTrait {}

class DemoClass {
	use DemoTrait;
}

?>
--EXPECTF--
Deprecated: Trait DemoTrait used by DemoClass is deprecated in %s on line %d
