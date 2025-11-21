--TEST--
Testing __debugInfo() magic method with bad returns scalar (debug_zval_dump)
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

$c = new C(true);
debug_zval_dump($c);

?>
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s on line %d
