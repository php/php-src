--TEST--
Testing __debugInfo() magic method with bad returns ONE
--FILE--
<?php

class C {
  public $val;
  public function __debugInfo() {
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
Fatal error: __debuginfo() must return an array in %s on line %d
