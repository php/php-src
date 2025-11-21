--TEST--
Testing __debugInfo() magic method with bad returns scalar inside an object (print_r)
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
print_r($o);
?>
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s on line %d
