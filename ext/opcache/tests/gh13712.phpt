--TEST--
GH-13712 (Segmentation fault for enabled observers when calling trait method of internal trait when opcache is loaded)
--EXTENSIONS--
opcache
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
class Foo {
	use _ZendTestTrait;
}

$f = new Foo();
var_dump($f->testMethod());
?>
--EXPECTF--
<!-- init '%s' -->
<!-- init Foo::testMethod() -->
<!-- init var_dump() -->
bool(true)
