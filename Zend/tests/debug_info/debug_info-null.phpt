--TEST--
Testing __debugInfo() magic method with deprecated return null
--FILE--
<?php

class Bar {
  public $val = 123;

  public function __debugInfo() {
    return null;
  }
}

$b = new Bar;
var_dump($b);
?>
--EXPECTF--
Deprecated: Returning null from Bar::__debugInfo() is deprecated, return an empty array instead in %s on line %d
object(Bar)#%d (0) {
}
