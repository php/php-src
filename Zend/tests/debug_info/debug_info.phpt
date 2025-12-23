--TEST--
Testing __debugInfo() magic method
--FILE--
<?php

class Foo {
  public $d = 4;
  protected $e = 5;
  private $f = 6;

  public function __debugInfo() {
    return ['a'=>1, "\0*\0b"=>2, "\0Foo\0c"=>3];
  }
}

$f = new Foo;
var_dump($f);

?>
--EXPECT--
object(Foo)#1 (3) {
  ["a"]=>
  int(1)
  ["b":protected]=>
  int(2)
  ["c":"Foo":private]=>
  int(3)
}
