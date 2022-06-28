--TEST--
Testing __debugInfo() magic method with bad returns OBJECT
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

$c = new C(new stdClass);
var_dump($c);
?>
--EXPECTF--
Fatal error: Uncaught Error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-object.php:14
Stack trace:
#0 %s(14): var_dump(Object(C))
#1 {main}
  thrown in %s on line 14
