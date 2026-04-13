--TEST--
Generic class: progressive inference — auto-freeze when min equals max
--FILE--
<?php
declare(strict_types=1);

class Container<T> {
    private array $items = [];
    public function add(T $item): void { $this->items[] = $item; }
    public function get(int $idx): T { return $this->items[$idx]; }
}

// === Auto-freeze: min == max after narrowing ===
echo "--- Auto-freeze via narrowing ---\n";

$c = new Container();
$c->add(1);         // min = int
$c->add(2);         // min = int (still)
echo "Before narrowing: added two ints\n";

function requireIntContainer(Container<int> $c): void {
    echo "Accepted as Container<int>\n";
}

// Passing to Container<int> narrows max to int. Since min is already int,
// min == max → auto-freeze to Container<int>
requireIntContainer($c);

// Now it should behave exactly like Container<int>
$c->add(3);
echo "After freeze: added int OK\n";

try {
    $c->add("text");
    echo "FAIL: should have thrown TypeError\n";
} catch (TypeError $e) {
    echo "After freeze: string rejected (OK)\n";
}

try {
    $c->add(3.14);
    echo "FAIL: should have thrown TypeError\n";
} catch (TypeError $e) {
    echo "After freeze: float rejected (OK)\n";
}

// === No auto-freeze when min != max ===
echo "--- No freeze when min < max ---\n";

$c2 = new Container();
$c2->add(1);         // min = int

function requireIntOrStringContainer(Container<int|string> $c): void {
    echo "Accepted as Container<int|string>\n";
}

// Narrows max to int|string, but min is just int → no freeze
requireIntOrStringContainer($c2);

// string is still allowed (within max)
$c2->add("hello");
echo "After narrowing: added string OK (within max)\n";

// float is outside max
try {
    $c2->add(3.14);
    echo "FAIL: should have thrown TypeError\n";
} catch (TypeError $e) {
    echo "After narrowing: float rejected (OK)\n";
}

// === Freeze with multiple types ===
echo "--- Freeze with union type ---\n";

$c3 = new Container();
$c3->add(1);         // min = int
$c3->add("text");    // min = int|string

// Narrowing to int|string with min already at int|string → freeze
requireIntOrStringContainer($c3);

$c3->add(42);
echo "Frozen int|string: added int OK\n";

$c3->add("more");
echo "Frozen int|string: added string OK\n";

try {
    $c3->add(3.14);
    echo "FAIL: should have thrown TypeError\n";
} catch (TypeError $e) {
    echo "Frozen int|string: float rejected (OK)\n";
}

echo "Done.\n";
?>
--EXPECT--
--- Auto-freeze via narrowing ---
Before narrowing: added two ints
Accepted as Container<int>
After freeze: added int OK
After freeze: string rejected (OK)
After freeze: float rejected (OK)
--- No freeze when min < max ---
Accepted as Container<int|string>
After narrowing: added string OK (within max)
After narrowing: float rejected (OK)
--- Freeze with union type ---
Accepted as Container<int|string>
Frozen int|string: added int OK
Frozen int|string: added string OK
Frozen int|string: float rejected (OK)
Done.
