--TEST--
Bug #31525 (object reference being dropped. $this getting lost)
--INI--
error_reporting=4095
--FILE--
<?php
class Foo {
  function getThis() {
    return $this;
  }
  function destroyThis() {
    $baz =& $this->getThis();
  }
}
$bar = new Foo();
$bar->destroyThis();
var_dump($bar);
?>
--EXPECTF--
Strict Standards: Only variables should be assigned by reference in %sbug31525.php on line 7
object(Foo)#1 (0) {
}
