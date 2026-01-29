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
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$m = new MultiOp();
var_dump($m->value);

// Decrement works too
class Decrement {
    public function __construct(
        public readonly int $value = 100,
    ) {
        $this->value--;
    }
}

$d = new Decrement();
var_dump($d->value);

// Assignment operators work
class AssignOps {
    public function __construct(
        public readonly string $text = 'hello',
    ) {
        $this->text .= ' world';
    }
}

$a = new AssignOps();
var_dump($a->text);

?>
--EXPECT--
int(6)
Cannot modify readonly property MultiOp::$value
int(15)
int(99)
string(11) "hello world"
