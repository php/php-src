--TEST--
gen_stub.php: nested namespaced typed properties test.
--EXTENSIONS--
zend_test
--FILE--
<?php

$foo = new \ZendTestNS3\Foo(new \ZendTestNS3\ZendSubNS\Foo());
var_dump($foo);
$foo->method1(new \ZendTestNS3\ZendSubNS\foo());
$foo->method2(new \ZendTestNS3\ZendSubNS\foo());
?>
--EXPECT--
object(ZendTestNS3\Foo)#1 (1) {
  ["foo"]=>
  object(ZendTestNS3\ZendSubNS\Foo)#2 (0) {
  }
}
