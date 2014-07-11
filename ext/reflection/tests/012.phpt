--TEST--
ReflectionClass::getDefaultProperties()
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
class Foo {
	public $test = "ok";
}
$class = new ReflectionClass("Foo");
$props = $class->getDefaultProperties();
echo $props["test"];
?>
--EXPECT--	
ok

