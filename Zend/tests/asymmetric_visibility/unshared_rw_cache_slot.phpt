--TEST--
R/w cache slots should be unshared
--FILE--
<?php

class P {
    public private(set) string $bar;
}

class C extends P {
    public function setBar($bar) {
        var_dump($this->bar);
        $this->bar = $bar;
    }
}

$c = new C();
try {
    $c->setBar(1);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $c->setBar(1);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($c);

?>
--EXPECTF--
Typed property P::$bar must not be accessed before initialization
Typed property P::$bar must not be accessed before initialization
object(C)#%d (0) {
  ["bar"]=>
  uninitialized(string)
}
