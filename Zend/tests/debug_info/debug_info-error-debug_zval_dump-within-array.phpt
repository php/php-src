--TEST--
Testing __debugInfo() magic method with bad returns scalar inside an array (debug_zval_dump)
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

$a = [
	'foo',
	new C(true),
	'bar',
];
debug_zval_dump($a);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: __debuginfo() must return an array in %s:%d
Stack trace:
#0 %s(%d): debug_zval_dump(Array)
#1 {main}
  thrown in %s on line %d
