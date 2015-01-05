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
base::base
derived::base
