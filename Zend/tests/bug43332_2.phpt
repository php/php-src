--TEST--
Bug #43332.2 (self and parent as type hint in namespace)
--FILE--
<?php
namespace foobar;

class foo {
  public function bar(\self $a) { }
}

$foo = new foo;
$foo->bar($foo); // Ok!
$foo->bar(new stdclass); // Error, ok!
--EXPECTF--
Fatal error: '\self' is an invalid class name in %sbug43332_2.php on line 5
