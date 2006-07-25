--TEST--
Reflection Bug #38132 (ReflectionClass::getStaticProperties() retains \0 in key names)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
class foo {
	static protected $bar = 'baz';
	static public $a = 'a';
}

$class = new ReflectionClass('foo');
$properties = $class->getStaticProperties();
var_dump($properties, array_keys($properties));
var_dump(isset($properties['*bar'])); // false
var_dump(isset($properties["\0*\0bar"])); // true

?>
--EXPECT--
array(2) {
  ["*bar"]=>
  string(3) "baz"
  ["a"]=>
  string(1) "a"
}
array(2) {
  [0]=>
  string(4) "*bar"
  [1]=>
  string(1) "a"
}
bool(true)
bool(false)
