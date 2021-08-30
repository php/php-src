--TEST--
gen_stub.php: nested namespaced typed properties test.
--EXTENSIONS--
zend_test
--FILE--
<?php

$foo = new \ZendTestNS2\Foo();
$foo->foo = new \ZendTestNS2\ZendSubNS\Foo();
var_dump($foo);
?>
--EXPECT--
object(ZendTestNS2\Foo)#1 (1) {
  ["foo"]=>
  object(ZendTestNS2\ZendSubNS\Foo)#2 (0) {
  }
}
