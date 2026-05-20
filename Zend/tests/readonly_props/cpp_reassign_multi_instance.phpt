--TEST--
Promoted readonly property reassignment - multiple instantiations (VM cache warm)
--FILE--
<?php

// Exercises the VM fast path (ASSIGN_OBJ cache warm on 2nd+ instantiation).
// Ensures the foreign-constructor scope check works on the cached path.

class P {
    public function __construct(
        public readonly string $x = 'P',
    ) {}
}

class C extends P {
    public function __construct(
        public readonly string $x = 'C',
    ) {
        try {
            parent::__construct();
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
    }
}

// First instantiation (slow path)
$c1 = new C();
var_dump($c1->x);

// Second instantiation (fast path - cache warm)
$c2 = new C();
var_dump($c2->x);

// Third for good measure
$c3 = new C();
var_dump($c3->x);

?>
--EXPECT--
Error: Cannot modify readonly property C::$x
string(1) "C"
Error: Cannot modify readonly property C::$x
string(1) "C"
Error: Cannot modify readonly property C::$x
string(1) "C"
