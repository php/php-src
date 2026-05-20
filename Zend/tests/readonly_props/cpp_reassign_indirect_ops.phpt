--TEST--
Promoted readonly property reassignment in constructor - indirect operations (++, --, +=)
--FILE--
<?php

// Test that indirect operations also work for promoted readonly properties
// Note: each operation (++, --, +=, etc.) consumes the one allowed reassignment

class Counter {
    public function __construct(
        public readonly int $count = 0,
    ) {
        // Single increment works
        $this->count++;
    }
}

$c = new Counter(5);
var_dump($c->count);

// Multiple operations count as reassignments - second fails
class MultiOp {
    public function __construct(
        public readonly int $value = 10,
    ) {
        $this->value += 5;  // First modification - allowed
        try {
            $this->value++;  // Second modification - should fail
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
    }
}

$m = new MultiOp();
var_dump($m->value);

?>
--EXPECT--
int(6)
Error: Cannot modify readonly property MultiOp::$value
int(15)
