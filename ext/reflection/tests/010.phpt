--TEST--
ReflectionMethod::__toString() tests (overriden method)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
class Foo {
	function func() {
	}
}
class Bar extends Foo {
	function func() {
	}
}
$m = new ReflectionMethod("Bar::func");
echo $m;
?>
--EXPECTF--
Method [ <user, overwrites Foo, prototype Foo> public method func ] {
  @@ %s010.php 7 - 8
}
