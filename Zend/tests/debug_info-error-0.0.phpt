--TEST--
Testing __debugInfo() magic method with bad returns ZERO (float)
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

$c = new C(0.0);
var_dump($c);
?>
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-0.0.php on line %d
