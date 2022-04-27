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
?>
--EXPECTF--
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
object(ZendTestNS2\Foo)#%d (%d) {
  ["foo"]=>
  uninitialized(ZendTestNS2\ZendSubNS\Foo)
}
object(ZendTestNS2\Foo)#%d (%d) {
  ["foo"]=>
  object(ZendTestNS2\ZendSubNS\Foo)#%d (%d) {
  }
}
