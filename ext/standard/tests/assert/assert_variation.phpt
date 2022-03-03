--TEST--
assert() - variation  - test callback options using ini_get/ini_set/assert_options
--INI--
assert.active = 1
assert.warning = 0
assert.callback = f1
assert.bail = 0
assert.exception=0
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
    static function assert($file, $line, $unused, $desc)
    {
        echo "Class assertion failed $line, \"$desc\"\n";
    }
}
echo "Initial values: assert_options(ASSERT_CALLBACK) => [".assert_options(ASSERT_CALLBACK)."]\n";
echo "Initial values: ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n";
var_dump($r2=assert(0 != 0));
echo"\n";

echo "Change callback function using ini.set and test return value \n";
var_dump($rv = ini_set("assert.callback","f2"));
echo "assert_options(ASSERT_CALLBACK) => [".assert_options(ASSERT_CALLBACK)."]\n";
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n";
var_dump($r2=assert(0 != 0));
echo"\n";

echo "Change callback function using assert_options and test return value \n";
var_dump($rv=assert_options(ASSERT_CALLBACK, "f3"));
echo "assert_options(ASSERT_CALLBACK) => [".assert_options(ASSERT_CALLBACK)."]\n";
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n";
var_dump($r2=assert(0 != 0));
echo"\n";


echo "Reset the name of the callback routine to a class method\n";
var_dump($rc=assert_options(ASSERT_CALLBACK, "c1"));
echo "assert_options(ASSERT_CALLBACK) => [".assert_options(ASSERT_CALLBACK)."]\n";
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n";
try {
    var_dump($r2=assert(0 != 0));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
echo"\n";

echo "Reset callback options to use a class method \n";
var_dump($rc = assert_options(ASSERT_CALLBACK,array("c1","assert")));
var_dump($rao=assert_options(ASSERT_CALLBACK));
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n\n";
var_dump($r2=assert(0 != 0));
echo"\n";

echo "Reset callback options to use an object method \n";
$o = new c1();
var_dump($rc=assert_options(ASSERT_CALLBACK,array(&$o,"assert")));
var_dump($rao=assert_options(ASSERT_CALLBACK));
echo "ini.get(\"assert.callback\") => [".ini_get("assert.callback")."]\n\n";
var_dump($r2=assert(0 != 0));
echo"\n";

echo "Set callback to something silly\n";
assert_options(ASSERT_CALLBACK, 3.141);
var_dump($rao = assert_options(ASSERT_CALLBACK));

?>
--EXPECT--
Initial values: assert_options(ASSERT_CALLBACK) => [f1]
Initial values: ini.get("assert.callback") => [f1]
f1 called
bool(false)

Change callback function using ini.set and test return value 
string(2) "f1"
assert_options(ASSERT_CALLBACK) => [f2]
ini.get("assert.callback") => [f2]
f2 called
bool(false)

Change callback function using assert_options and test return value 
string(2) "f2"
assert_options(ASSERT_CALLBACK) => [f3]
ini.get("assert.callback") => [f2]
f3 called
bool(false)

Reset the name of the callback routine to a class method
string(2) "f3"
assert_options(ASSERT_CALLBACK) => [c1]
ini.get("assert.callback") => [f2]
Invalid callback c1, function "c1" not found or invalid function name

Reset callback options to use a class method 
string(2) "c1"
array(2) {
  [0]=>
  string(2) "c1"
  [1]=>
  string(6) "assert"
}
ini.get("assert.callback") => [f2]

Class assertion failed 56, "assert(0 != 0)"
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
  &object(c1)#2 (0) {
  }
  [1]=>
  string(6) "assert"
}
ini.get("assert.callback") => [f2]

Class assertion failed 64, "assert(0 != 0)"
bool(false)

Set callback to something silly
float(3.141)
