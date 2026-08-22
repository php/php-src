--TEST--
Unset from __unset respects set visibility
--FILE--
<?php

class C {
    public private(set) int $a = 1;
    public function __construct() {
        unset($this->a);
    }
}

class D extends C {
    public function __unset($name) {
        unset($this->a);
    }
}

$c = new D();
try {
    unset($c->a);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($c);

?>
--EXPECTF--
Error: Cannot unset private(set) property C::$a from scope D
object(D)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
