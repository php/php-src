--TEST--
#[\Deprecated]: Instanceof, type checking, etc. for deprecated interfaces are okay
--FILE--
<?php

#[\Deprecated("please do not use")]
interface DemoInterface {}

class Identity {
	public static function identity(
		DemoInterface $in
	): DemoInterface {
		return $in;
	}
}

class ImplementsDemo implements DemoInterface {};

$instance = new ImplementsDemo();

var_dump(is_a(ImplementsDemo::class, DemoInterface::class, true));
var_dump(is_a($instance, DemoInterface::class));
var_dump(is_subclass_of(Implementsdemo::class, DemoInterface::class, true));
var_dump(is_subclass_of($instance, DemoInterface::class));
var_dump($instance === Identity::identity($instance));
var_dump($instance instanceof DemoInterface);
?>
--EXPECTF--
Deprecated: Interface DemoInterface implemented by ImplementsDemo is deprecated, please do not use in %s on line %d
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
