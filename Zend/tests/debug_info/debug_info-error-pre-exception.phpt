--TEST--
__debugInfo() with pre-existing exception does not emit a duplicate exception
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

try {
    var_dump(new C(0), new C(['test' => 0]));
} catch(TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
object(C)#%d (0) {
}
object(C)#%d (0) {
}
TypeError: C::__debugInfo() must return an array, int returned
