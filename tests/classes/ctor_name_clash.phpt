--TEST--
ZE2 The child class can re-use the parent class name for a function member
--FILE--
<?php
class base {
  function base() {
    echo __CLASS__."::".__FUNCTION__."\n";
  }
}

class derived extends base {
  function base() {
    echo __CLASS__."::".__FUNCTION__."\n";
  }
}

$obj = new derived();
$obj->base();
?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; base has a deprecated constructor in %s on line %d
base::base
derived::base
