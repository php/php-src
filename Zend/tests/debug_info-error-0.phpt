--TEST--
Testing __debugInfo() magic method with bad returns ZERO
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

$c = new C(0);
var_dump($c);
?>
--EXPECTF--
Fatal error: Uncaught Error: __debuginfo() must return an array in __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-0.php:14
Stack trace:
#0 %s(14): var_dump(Object(C))
#1 {main}
  thrown in %s on line 14
