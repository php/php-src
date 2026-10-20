--TEST--
#[\Deprecated]: Using multiple traits
--FILE--
<?php

#[\Deprecated]
trait DemoTraitA {}

#[\Deprecated]
trait DemoTraitB {}

trait DemoTraitC {}

class DemoClass {
	use DemoTraitA, DemoTraitB, DemoTraitC;
}

?>
--EXPECTF--
Deprecated: Trait DemoTraitA used by DemoClass is deprecated in %s on line %d

Deprecated: Trait DemoTraitB used by DemoClass is deprecated in %s on line %d
