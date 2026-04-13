--TEST--
Generic class: progressive inference — clone preserves state independently
--FILE--
<?php
declare(strict_types=1);

class Stack<T> {
    private array $items = [];
    public function push(T $item): void { $this->items[] = $item; }
    public function count(): int { return count($this->items); }
}

// === Clone progressive object ===
echo "--- Clone progressive state ---\n";

$s1 = new Stack();
$s1->push(1);
$s1->push(2);
echo "Original: pushed 2 ints\n";

// Narrow original
function requireIntStack(Stack<int> $s): void {
    echo "Accepted as Stack<int>\n";
}
requireIntStack($s1);

// Clone AFTER narrowing
$s2 = clone $s1;
echo "Cloned\n";

// Original is frozen to int
$s1->push(3);
echo "Original: pushed int OK\n";

try {
    $s1->push("text");
    echo "FAIL: original should reject string\n";
} catch (TypeError $e) {
    echo "Original: string rejected (OK)\n";
}

// Clone should also be frozen to int (inherited frozen state)
$s2->push(4);
echo "Clone: pushed int OK\n";

try {
    $s2->push("text");
    echo "FAIL: clone should reject string\n";
} catch (TypeError $e) {
    echo "Clone: string rejected (OK)\n";
}

// === Clone before narrowing — independent state ===
echo "--- Clone before narrowing ---\n";

$a = new Stack();
$a->push(1);
echo "a: pushed int\n";

$b = clone $a;
echo "Cloned a -> b\n";

// Narrow a to int
requireIntStack($a);

// a should reject string
try {
    $a->push("text");
    echo "FAIL: a should reject string\n";
} catch (TypeError $e) {
    echo "a: string rejected after narrowing (OK)\n";
}

// b was cloned BEFORE narrowing, so it still has its own progressive state
// b's min is int but max is still mixed (un-narrowed)
$b->push("text");
echo "b: pushed string OK (independent state)\n";

$b->push(3.14);
echo "b: pushed float OK (independent state)\n";

echo "Done.\n";
?>
--EXPECT--
--- Clone progressive state ---
Original: pushed 2 ints
Accepted as Stack<int>
Cloned
Original: pushed int OK
Original: string rejected (OK)
Clone: pushed int OK
Clone: string rejected (OK)
--- Clone before narrowing ---
a: pushed int
Cloned a -> b
Accepted as Stack<int>
a: string rejected after narrowing (OK)
b: pushed string OK (independent state)
b: pushed float OK (independent state)
Done.
