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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
Error: Cannot acquire reference to readonly property C::$prop
