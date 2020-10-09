--TEST--
Testing __debugInfo() magic method with bad returns OBJECT
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

$c = new C(new stdClass);
var_dump($c);
?>
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-object.php on line %d
