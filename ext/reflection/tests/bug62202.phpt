--TEST--
Bug #62202 (ReflectionParameter::getDefaultValue() leaks with constant)
--FILE--
<?php

define('TEST_CONST', '10');

class Foo {
	public function bar($param = TEST_CONST) {
		return $param;
	}
}

$refl_method = new ReflectionMethod('Foo::bar');

foreach ($refl_method->getParameters() as $param) {
	if ($param->isDefaultValueAvailable()) {
		var_dump($param->getDefaultValue());
	}
}
?>
--EXPECT--
string(2) "10"