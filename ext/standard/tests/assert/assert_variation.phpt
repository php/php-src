--TEST--
assert() - variation  - test callback options using ini_get/ini_set/assert_options
--INI--
assert.active = 1
assert.warning = 0
assert.callback = f1
assert.bail = 0
assert.quiet_eval = 0
--FILE--
<?php
function f1()
{
	echo "f1 called\n";
}
function f2()
{
	echo "f2 called\n";
}
function f3()
{
	echo "f3 called\n";
}
class c1
{
	function assert($file, $line, $myev)
	{
		echo "Class assertion failed $line, \"$myev\"\n";
	}
}
echo "Initial values: assert_options(ASSERT_CALLBACK) => [".assert_options(ASSERT_CALLBACK)."]\n";
echo "Initial values: ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n";
$sa = "0 != 0";
var_dump($r2=assert($sa));
echo"\n";

echo "Change callback function using ini.set and test return value \n";
var_dump($rv = ini_set("assert.callback","f2"));
echo "assert_options(ASSERT_CALLBACK) => [".assert_options(ASSERT_CALLBACK)."]\n";
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n";
var_dump($r2=assert($sa));
echo"\n";

echo "Change callback function using assert_options and test return value \n";
var_dump($rv=assert_options(ASSERT_CALLBACK, "f3"));
echo "assert_options(ASSERT_CALLBACK) => [".assert_options(ASSERT_CALLBACK)."]\n";
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n";
var_dump($r2=assert($sa));
echo"\n";


echo "Reset the name of the callback routine to a class method and check that it works\n";
var_dump($rc=assert_options(ASSERT_CALLBACK, "c1"));
echo "assert_options(ASSERT_CALLBACK) => [".assert_options(ASSERT_CALLBACK)."]\n";
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n";
var_dump($r2=assert($sa));
echo"\n";

echo "Reset callback options to use a class method \n";
var_dump($rc = assert_options(ASSERT_CALLBACK,array("c1","assert")));
var_dump($rao=assert_options(ASSERT_CALLBACK));
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n\n";
var_dump($r2=assert($sa));
echo"\n";

echo "Reset callback options to use an object method \n";
$o = new c1();
var_dump($rc=assert_options(ASSERT_CALLBACK,array(&$o,"assert")));
var_dump($rao=assert_options(ASSERT_CALLBACK));
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n\n";
var_dump($r2=assert($sa));
echo"\n";
--EXPECTF--
Initial values: assert_options(ASSERT_CALLBACK) => [f1]
Initial values: ini.get("assert.callback") => [f1]

Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d
f1 called
bool(false)

Change callback function using ini.set and test return value 
string(2) "f1"
assert_options(ASSERT_CALLBACK) => [f2]
ini.get("assert.callback") => [f2]

Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d
f2 called
bool(false)

Change callback function using assert_options and test return value 
string(2) "f2"
assert_options(ASSERT_CALLBACK) => [f3]
ini.get("assert.callback") => [f2]

Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d
f3 called
bool(false)

Reset the name of the callback routine to a class method and check that it works
string(2) "f3"
assert_options(ASSERT_CALLBACK) => [c1]
ini.get("assert.callback") => [f2]

Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d
bool(false)

Reset callback options to use a class method 
string(2) "c1"
array(2) {
  [0]=>
  string(2) "c1"
  [1]=>
  string(6) "assert"
}
ini.get("assert.callback") => [f2]


Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d

Deprecated: Non-static method c1::assert() should not be called statically in %s on line 53
Class assertion failed 53, "0 != 0"
bool(false)

Reset callback options to use an object method 
array(2) {
  [0]=>
  string(2) "c1"
  [1]=>
  string(6) "assert"
}
array(2) {
  [0]=>
  &object(c1)#1 (0) {
  }
  [1]=>
  string(6) "assert"
}
ini.get("assert.callback") => [f2]


Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d
Class assertion failed 61, "0 != 0"
bool(false)
