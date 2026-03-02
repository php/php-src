--TEST--
#[\Deprecated]: Message from protected class constant.
--FILE--
<?php

class P {
	protected const DEPRECATION_MESSAGE = 'from class constant';
}

class Clazz extends P {
	#[\Deprecated(parent::DEPRECATION_MESSAGE)]
	public function test() {

	}
}

$c = new Clazz();
$c->test();

?>
--EXPECTF--
Deprecated: Method Clazz::test() is deprecated, from class constant in %s on line %d
