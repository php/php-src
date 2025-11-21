--TEST--
Testing __debugInfo() magic method with bad returns scalar inside an object
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
var_dump($o);
?>
--EXPECTF--
object(stdClass)#1 (3) {
  ["foo"]=>
  string(3) "foo"
  ["c"]=>
  
Fatal error: __debuginfo() must return an array in %s on line %d
