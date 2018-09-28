--TEST--
Using "parent" in class without parent results in compile-time error
--FILE--
<?php
class InvalidClass {
  function method() {
    echo parent::class;
  }
}

?>
--EXPECTF--
Deprecated: Cannot use "parent" without a parent class in %s on line %d
