--TEST--
Bug #43332.1 (self and parent as type hint in namespace)
--FILE--
<?php
namespace foobar;

class foo {
  public function bar(self $a) { }
}

$foo = new foo;
$foo->bar($foo); // Ok!
$foo->bar(new \stdclass); // Error, ok!
--EXPECTF--
Catchable fatal error: Argument 1 passed to foobar\foo::bar() must be an instance of foobar\foo, instance of stdClass given, called in %sbug43332_1.php on line 10 and defined in %sbug43332_1.php on line 5
