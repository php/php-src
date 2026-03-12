--TEST--
Promoted readonly property reassignment through plain function should error
--FILE--
<?php

class C {
    public function __construct(public readonly int $prop) {
        set($this);
    }

    public function set() {
        $this->prop++;
    }
}

function set($c) {
    try {
        $c->set();
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

$c = new C(42);
var_dump($c->prop);

?>
--EXPECT--
Error: Cannot modify readonly property C::$prop
int(42)
