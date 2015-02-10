--TEST--
ReflectionParameter::getDefaultValue() with constants and non-compile-time ASTs
--FILE--
<?php

define("CONST_TEST_1", "const1");

function ReflectionParameterTest($test1=array(), $test2 = CONST_TEST_1) {
	echo $test;
}
$reflect = new ReflectionFunction('ReflectionParameterTest');
foreach($reflect->getParameters() as $param) {
	if($param->isDefaultValueAvailable()) {
		var_dump($param->getDefaultValue());
	}
}

class Foo2 {
	const bar = 'Foo2::bar';
}

class Foo {
	const bar = 'Foo::bar';

	public function baz($param1 = self::bar, $param2 = Foo2::bar, $param3 = CONST_TEST_1 . "+string") {
	}
}

$method = new ReflectionMethod('Foo', 'baz');
$params = $method->getParameters();

foreach ($params as $param) {
    if ($param->isDefaultValueAvailable()) {
        var_dump($param->getDefaultValue());
    }
}
?>
==DONE==
--EXPECT--
array(0) {
}
string(6) "const1"
string(8) "Foo::bar"
string(9) "Foo2::bar"
string(13) "const1+string"
==DONE==
