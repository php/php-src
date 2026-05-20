--TEST--
Promoted readonly property reassignment - child can reassign after parent::__construct()
--FILE--
<?php

// When both parent and child have CPP for the same property, the child's
// reassignment window must survive the parent constructor's exit cleanup.

class P {
    public function __construct(
        public readonly string $x = 'P_default',
    ) {
        // Parent does NOT reassign
    }
}

class C extends P {
    public function __construct(
        public readonly string $x = 'C_default',
    ) {
        try {
            parent::__construct();
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
        // Child should still be able to reassign its own CPP property
        $this->x = 'C_reassigned';
    }
}

$c = new C();
var_dump($c->x);

// Also test with multiple instances
$c2 = new C();
var_dump($c2->x);

?>
--EXPECT--
Error: Cannot modify readonly property C::$x
string(12) "C_reassigned"
Error: Cannot modify readonly property C::$x
string(12) "C_reassigned"
