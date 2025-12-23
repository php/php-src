--TEST--
Testing __debugInfo() magic method with bad returns scalar inside an array (print_r)
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
print_r($a);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: __debuginfo() must return an array in %s:%d
Stack trace:
#0 %s(%d): print_r(Array)
#1 {main}
  thrown in %s on line %d
