--TEST--
gen_stub.php: nested namespaced typed properties test.
--EXTENSIONS--
zend_test
--FILE--
<?php

$foo = new \ZendTestNS2\Foo();
var_dump($foo);
$foo->foo = new \ZendTestNS2\ZendSubNS\Foo();
var_dump($foo);
$foo = new \ZendTestNS\UnlikelyCompileError();
var_dump($foo);
?>
--EXPECTF--
object(ZendTestNS2\Foo)#%d (%d) {
  ["foo"]=>
  uninitialized(ZendTestNS2\ZendSubNS\Foo)
}
object(ZendTestNS2\Foo)#%d (%d) {
  ["foo"]=>
  object(ZendTestNS2\ZendSubNS\Foo)#%d (%d) {
  }
}
object(ZendTestNS\UnlikelyCompileError)#%d (%d) {
}
