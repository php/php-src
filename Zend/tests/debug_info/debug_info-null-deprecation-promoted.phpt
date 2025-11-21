--TEST--
Testing __debugInfo() magic method with deprecated return null
--FILE--
<?php

set_error_handler(function($_, $m) {
    throw new Exception($m);
});

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
object(Bar)#2 (0) {
}

Fatal error: Uncaught Exception: Returning null from Bar::__debugInfo() is deprecated, return an empty array instead in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(8192, 'Returning null ...', '%s', %d)
#1 %s(%d): var_dump(Object(Bar))
#2 {main}
  thrown in %s on line %d
