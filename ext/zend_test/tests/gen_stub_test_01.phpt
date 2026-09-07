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
$foo = new \ZendTestNS\NotUnlikelyCompileError();
var_dump($foo);
?>
--EXPECTF--
object(ZendTestNS2\Foo)#%d (%d) {
  ["foo"]=>
  uninitialized(ZendTestNS2\ZendSubNS\Foo)
  ["intersectionProp"]=>
  uninitialized(ZendTestNS2\ZendSubNS\Foo&ZendTestNS\Bar)
  ["unionProp"]=>
  uninitialized(ZendTestNS2\ZendSubNS\Foo|ZendTestNS\Bar)
  ["fooAlias"]=>
  uninitialized(ZendTestNS\Foo)
  ["unlProp"]=>
  uninitialized(ZendTestNS\UnlikelyCompileError)
  ["notUnlProp"]=>
  uninitialized(ZendTestNS\NotUnlikelyCompileError)
}
object(ZendTestNS2\Foo)#%d (%d) {
  ["foo"]=>
  object(ZendTestNS2\ZendSubNS\Foo)#%d (%d) {
  }
  ["intersectionProp"]=>
  uninitialized(ZendTestNS2\ZendSubNS\Foo&ZendTestNS\Bar)
  ["unionProp"]=>
  uninitialized(ZendTestNS2\ZendSubNS\Foo|ZendTestNS\Bar)
  ["fooAlias"]=>
  uninitialized(ZendTestNS\Foo)
  ["unlProp"]=>
  uninitialized(ZendTestNS\UnlikelyCompileError)
  ["notUnlProp"]=>
  uninitialized(ZendTestNS\NotUnlikelyCompileError)
}
object(ZendTestNS\UnlikelyCompileError)#%d (%d) {
}
object(ZendTestNS\NotUnlikelyCompileError)#%d (%d) {
}
