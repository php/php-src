--TEST--
Set from __set respects set visibility
--FILE--
<?php

class C {
    public private(set) int $a = 1;
    public function __construct() {
        unset($this->a);
    }
}

class D extends C {
    public function __set($name, $value) {
        $this->a = $value;
    }
}

$c = new D();
try {
    $c->a = 2;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($c);

?>
--EXPECTF--
Error: Cannot modify private(set) property C::$a from scope D
object(D)#%d (0) {
  ["a"]=>
  uninitialized(int)
}
