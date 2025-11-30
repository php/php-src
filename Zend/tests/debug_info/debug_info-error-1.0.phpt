--TEST--
Testing __debugInfo() magic method with bad returns ONE (float)
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

$c = new C(1.0);
var_dump($c);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: __debuginfo() must return an array in %s:%d
Stack trace:
#0 %s(%d): var_dump(Object(C))
#1 {main}
  thrown in %s on line %d
