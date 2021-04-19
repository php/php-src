--TEST--
Partial application variation GC
--FILE--
<?php
class Foo {

  public function __construct() {
    $this->method = self::__construct(...);
  }
}

$foo = new Foo;
$foo->bar = $foo;

echo "OK";
?>
--EXPECT--
OK
