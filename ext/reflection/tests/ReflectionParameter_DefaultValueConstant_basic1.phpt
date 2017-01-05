--TEST--
ReflectionParameter::isDefaultValueConstant() && getDefaultValueConstantName()
--FILE--
<?php

define("CONST_TEST_1", "const1");

function ReflectionParameterTest($test1=array(), $test2 = CONST_TEST_1, $test3 = CASE_LOWER) {
	echo $test;
}
$reflect = new ReflectionFunction('ReflectionParameterTest');
foreach($reflect->getParameters() as $param) {
	if($param->getName() == 'test1') {
		var_dump($param->isDefaultValueConstant());
	}
	if($param->getName() == 'test2') {
		var_dump($param->isDefaultValueConstant());
	}
	if($param->isDefaultValueAvailable() && $param->isDefaultValueConstant()) {
		var_dump($param->getDefaultValueConstantName());
	}
}

class Foo2 {
	const bar = 'Foo2::bar';
}

class Foo {
	const bar = 'Foo::bar';

	public function baz($param1 = self::bar, $param2=Foo2::bar, $param3=CONST_TEST_1) {
	}
}

$method = new ReflectionMethod('Foo', 'baz');
$params = $method->getParameters();

foreach ($params as $param) {
    if ($param->isDefaultValueConstant()) {
        var_dump($param->getDefaultValueConstantName());
    }
}
?>
==DONE==
--EXPECT--
bool(false)
bool(true)
string(12) "CONST_TEST_1"
string(10) "CASE_LOWER"
string(9) "self::bar"
string(9) "Foo2::bar"
string(12) "CONST_TEST_1"
==DONE==
