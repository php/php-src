--TEST--
#[\Deprecated]: Trait using a deprecated trait
--FILE--
<?php

#[\Deprecated]
trait DemoTraitA {}

trait DemoTraitB {
	use DemoTraitA;
}

class DemoClass {
	use DemoTraitB;
}

?>
--EXPECTF--
Deprecated: Trait DemoTraitA used by DemoTraitB is deprecated in %s on line %d
