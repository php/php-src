--TEST--
GH-16185: Hooked object iterator with readonly props
--FILE--
<?php

class C {
    public readonly int $prop;
    public $dummy { set {} }

    public function init() {
        $this->prop = 1;
    }
}

$c = new C;

// Okay, as foreach skips over uninitialized properties.
foreach ($c as &$prop) {}

$c->init();

try {
    foreach ($c as &$prop) {}
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Cannot acquire reference to readonly property C::$prop
