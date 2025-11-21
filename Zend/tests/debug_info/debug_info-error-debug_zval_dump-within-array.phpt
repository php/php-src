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
array(3) packed refcount(2){
  [0]=>
  string(3) "foo" interned
  [1]=>
  
Fatal error: __debuginfo() must return an array in %s on line %d
