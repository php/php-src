--TEST--
Testing __debugInfo() magic method with bad returns STRING
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

$c = new C("foo");
var_dump($c);
?>
--EXPECTF--
Fatal error: Uncaught Error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-str.php:14
Stack trace:
#0 %s(14): var_dump(Object(C))
#1 {main}
  thrown in %s on line 14
