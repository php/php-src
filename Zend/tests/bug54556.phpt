--TEST--
Bug #54556 (array access to empty var does not trigger a notice)
--FILE--
<?php
class Foo {
  private $bar;
  function nonotice(){
    var_dump($this->bar['yeah']);
  }
}

$foo = new Foo();
$foo->nonotice();
?>
--EXPECTF--
Notice: Cannot get offset of a non-array variable in %s on line %d
NULL
