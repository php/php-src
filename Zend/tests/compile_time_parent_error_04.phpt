--TEST--
Using "parent" in class without parent results in compile-time error
--FILE--
<?php
$obj = new class() {
  function method() {
    echo parent::class;
  }
}

?>
--EXPECTF--
Deprecated: Cannot use "parent" without a parent class in %s on line %d
