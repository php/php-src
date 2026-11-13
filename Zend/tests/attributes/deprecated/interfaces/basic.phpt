--TEST--
#[\Deprecated]: Basic interface deprecation
--FILE--
<?php

#[\Deprecated("please do not use")]
interface DemoInterface1 {}

#[\Deprecated("will be removed in 3.0", since: "2.7")]
interface DemoInterface2 {}

#[\Deprecated(message: "going away")]
interface DemoInterface3 {}

#[\Deprecated(since: "3.5")]
interface DemoInterface4 {}

class DemoClass implements
	DemoInterface1,
	DemoInterface2,
	DemoInterface3,
	DemoInterface4
{
}

?>
--EXPECTF--
Deprecated: Interface DemoInterface1 implemented by DemoClass is deprecated, please do not use in %s on line %d

Deprecated: Interface DemoInterface2 implemented by DemoClass is deprecated since 2.7, will be removed in 3.0 in %s on line %d

Deprecated: Interface DemoInterface3 implemented by DemoClass is deprecated, going away in %s on line %d

Deprecated: Interface DemoInterface4 implemented by DemoClass is deprecated since 3.5 in %s on line %d
