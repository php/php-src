--TEST--
Testing __debugInfo() magic method with bad returns STRING
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

$c = new C("foo");
try {
    var_dump($c);
} catch(TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
object(C)#%d (0) {
}
TypeError: C::__debugInfo() must return an array, string returned
