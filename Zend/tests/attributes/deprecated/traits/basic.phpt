--TEST--
#[\Deprecated]: Basic trait deprecation
--FILE--
<?php

#[\Deprecated("please do not use")]
trait DemoTrait1 {}

#[\Deprecated("will be removed in 3.0", since: "2.7")]
trait DemoTrait2 {}

#[\Deprecated(message: "going away")]
trait DemoTrait3 {}

#[\Deprecated(since: "3.5")]
trait DemoTrait4 {}

class DemoClass {
	use DemoTrait1;
	use DemoTrait2;
	use DemoTrait3;
	use DemoTrait4;
}

?>
--EXPECTF--
Deprecated: Trait DemoTrait1 used by DemoClass is deprecated, please do not use in %s on line %d

Deprecated: Trait DemoTrait2 used by DemoClass is deprecated since 2.7, will be removed in 3.0 in %s on line %d

Deprecated: Trait DemoTrait3 used by DemoClass is deprecated, going away in %s on line %d

Deprecated: Trait DemoTrait4 used by DemoClass is deprecated since 3.5 in %s on line %d
