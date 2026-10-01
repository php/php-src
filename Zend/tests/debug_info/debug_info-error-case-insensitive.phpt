--TEST--
Testing __debugInfo() magic method declared with non-canonical case
--FILE--
<?php

class C {
  public $val;
  public function __DEBUGINFO() {
    return $this->val;
  }
  public function __construct($val) {
    $this->val = $val;
  }
}

$c = new C(1);
var_dump($c);
?>
--EXPECTF--
Fatal error: __debugInfo() must return an array in %s on line %d
