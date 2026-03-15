--TEST--
Generic class: progressive inference — min-type widening and max-type narrowing
--FILE--
<?php
declare(strict_types=1);

class Collection<T> {
    private array $items = [];
    public function add(T $item): void { $this->items[] = $item; }
    public function getAll(): array { return $this->items; }
}

// === Progressive mode: no type specified ===
echo "--- Progressive widening ---\n";
$c = new Collection();

// Min starts at never, max at mixed — anything goes
$c->add(1);
echo "Added int: OK\n";

$c->add(2);
echo "Added another int: OK\n";

$c->add("text");
echo "Added string: OK (min widens to int|string)\n";

// Before any narrowing, all types are still accepted
$c->add(3.14);
echo "Added float: OK (max still mixed)\n";

echo "Count: " . count($c->getAll()) . "\n";

// === Max-type narrowing via typed function ===
echo "--- Max-type narrowing ---\n";

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function set(T $value): void { $this->value = $value; }
}

function acceptIntCollection(Collection<int|string> $c): void {
    echo "Accepted collection\n";
}

$c2 = new Collection();
$c2->add(1);
$c2->add("hello");
echo "Before narrowing: added int and string\n";

// Passing to typed function narrows max to int|string
acceptIntCollection($c2);

// int is still allowed (within max)
$c2->add(42);
echo "After narrowing: added int OK\n";

// string is still allowed
$c2->add("world");
echo "After narrowing: added string OK\n";

// float is now outside max (int|string) — should fail
try {
    $c2->add(3.14);
    echo "FAIL: should have thrown TypeError\n";
} catch (TypeError $e) {
    echo "After narrowing: float rejected (OK)\n";
}

// array is also outside max
try {
    $c2->add([1, 2]);
    echo "FAIL: should have thrown TypeError\n";
} catch (TypeError $e) {
    echo "After narrowing: array rejected (OK)\n";
}

// === Frozen types still work ===
echo "--- Frozen explicit type ---\n";
$frozen = new Collection<int>();
$frozen->add(1);
$frozen->add(2);
echo "Frozen Collection<int>: added ints OK\n";

try {
    $frozen->add("text");
} catch (TypeError $e) {
    echo "Frozen Collection<int>: string rejected (OK)\n";
}

// === Frozen inferred type ===
echo "--- Frozen inferred type ---\n";
$box = new Box(42);
echo "Inferred Box<int>: " . $box->value . "\n";

try {
    $box->set("hello");
} catch (TypeError $e) {
    echo "Inferred Box<int>: string rejected (OK)\n";
}

echo "Done.\n";
?>
--EXPECT--
--- Progressive widening ---
Added int: OK
Added another int: OK
Added string: OK (min widens to int|string)
Added float: OK (max still mixed)
Count: 4
--- Max-type narrowing ---
Before narrowing: added int and string
Accepted collection
After narrowing: added int OK
After narrowing: added string OK
After narrowing: float rejected (OK)
After narrowing: array rejected (OK)
--- Frozen explicit type ---
Frozen Collection<int>: added ints OK
Frozen Collection<int>: string rejected (OK)
--- Frozen inferred type ---
Inferred Box<int>: 42
Inferred Box<int>: string rejected (OK)
Done.
