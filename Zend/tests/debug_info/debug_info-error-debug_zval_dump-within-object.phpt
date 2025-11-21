--TEST--
Testing __debugInfo() magic method with bad returns scalar inside an object (debug_zval_dump)
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

$o = new stdClass();
$o->foo = 'foo';
$o->c = new C(true);
$o->bar = 'bar';
debug_zval_dump($o);
?>
--EXPECTF--
object(stdClass)#1 (3) refcount(2){
  ["foo"]=>
  string(3) "foo" interned
  ["c"]=>
  
Fatal error: __debuginfo() must return an array in %s on line %d
