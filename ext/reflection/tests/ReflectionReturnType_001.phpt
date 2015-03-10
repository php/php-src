--TEST--
ReflectionReturnType - basic
--FILE--
<?php

class TestClass {}

function dump_rt_obj($rt) {
	var_dump($rt->isArray());
	var_dump($rt->isCallable());
	var_dump($rt->allowsNull());
	var_dump($rt->isReturnByReference());
	var_dump($rt->getClass());

	echo "=========\n";
}

function user_no_return_type() {

}

function user_return_array() : array {
	return [];
}

function user_return_callable() : callable {
	return function () {};
}

function user_return_class() : TestClass {
	return new TestClass();
}

function &user_return_class2() : TestClass {
	return new TestClass();
}

echo "Basic:----------\n";
echo ReflectionReturnType::export("user_return_class", true) . "\n";
echo ReflectionReturnType::export("user_return_class2", true) . "\n";

echo "No return types:-----------\n";
$rf = new ReflectionFunction('user_no_return_type');
var_dump($rf->getReturnType());

$rf = new ReflectionFunction('strlen');
var_dump($rf->getReturnType());

$rf = new ReflectionFunction(function() {});
var_dump($rf->getReturnType());

try {
	new ReflectionReturnType("None-exist-function");
} catch(Exception $e) {
	echo "Should not generate return type from none-exist function\n";
}

echo "With return types:-----------\n";

$rf = new ReflectionFunction("user_return_callable");
$rt = $rf->getReturnType();
dump_rt_obj($rt);

$rt = new ReflectionReturnType("user_return_array");
dump_rt_obj($rt);

$rt = new ReflectionReturnType("user_return_class");
dump_rt_obj($rt);

$rt = new ReflectionReturnType("user_return_class2");
dump_rt_obj($rt);
echo $rt . "\n";

$rt = new ReflectionReturnType(function() : array { return []; });
dump_rt_obj($rt);

echo $rt . "\n";

?>
--EXPECTF--
Basic:----------
ReturnType [ TestClass  ]
ReturnType [ TestClass & ]
No return types:-----------
bool(false)
bool(false)
bool(false)
Should not generate return type from none-exist function
With return types:-----------
bool(false)
bool(true)
bool(false)
bool(false)
NULL
=========
bool(true)
bool(false)
bool(false)
bool(false)
NULL
=========
bool(false)
bool(false)
bool(false)
bool(false)
object(ReflectionClass)#3 (1) {
  ["name"]=>
  string(9) "TestClass"
}
=========
bool(false)
bool(false)
bool(false)
bool(true)
object(ReflectionClass)#1 (1) {
  ["name"]=>
  string(9) "TestClass"
}
=========
ReturnType [ TestClass & ]
bool(true)
bool(false)
bool(false)
bool(false)
NULL
=========
ReturnType [ array  ]
