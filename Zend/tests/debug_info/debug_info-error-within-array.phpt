--TEST--
Testing __debugInfo() magic method with bad returns scalar inside an array
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
var_dump($a);
?>
--EXPECTF--
array(3) {
  [0]=>
  string(3) "foo"
  [1]=>
  
Fatal error: __debuginfo() must return an array in %s on line %d
