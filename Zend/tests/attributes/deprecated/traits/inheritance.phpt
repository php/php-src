--TEST--
#[\Deprecated]: Deprecated traits only apply to direct use, not inheritance
--FILE--
<?php

#[\Deprecated]
trait DemoTrait {}

class DemoClass {
	use DemoTrait;
}

class ChildClass extends DemoClass {
}

?>
--EXPECTF--
Deprecated: Trait DemoTrait used by DemoClass is deprecated in %s on line %d
