--TEST--
Testing __debugInfo() magic method declared with non-canonical case
--FILE--
<?php

class C {
  public $val;
  public function __DEBUGINFO() {
    return $this->val;
  }
  public function __construct($val) {
    $this->val = $val;
  }
}

$c = new C(1);
try {
    var_dump($c);
} catch(TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
object(C)#%d (0) {
}
TypeError: C::__debugInfo() must return an array, int returned
