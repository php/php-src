--TEST--
Generic class: inline cache for generic class type hints in function signatures
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// Function with generic class type hint — exercises inline CE + args cache
function acceptBoxInt(Box<int> $box): int {
    return $box->get();
}

function acceptBoxString(Box<string> $box): string {
    return $box->get();
}

// First call populates the cache
$b1 = new Box<int>(10);
echo acceptBoxInt($b1) . "\n";

// Subsequent calls should hit the monomorphic cache
$b2 = new Box<int>(20);
$b3 = new Box<int>(30);
echo acceptBoxInt($b2) . "\n";
echo acceptBoxInt($b3) . "\n";

// Inferred args should also match cache (interned to same pointer)
$b4 = new Box(40);
echo acceptBoxInt($b4) . "\n";

// Different generic type — should not interfere
$s1 = new Box<string>("hello");
echo acceptBoxString($s1) . "\n";

// Wrong type should still be rejected
try {
    acceptBoxInt(new Box<string>("wrong"));
} catch (TypeError $e) {
    echo "rejected wrong type\n";
}

// Return type with generic class
function makeBoxInt(int $v): Box<int> {
    return new Box<int>($v);
}

$r = makeBoxInt(99);
echo $r->get() . "\n";

// Multiple calls to exercise return type cache
echo makeBoxInt(100)->get() . "\n";
echo makeBoxInt(101)->get() . "\n";

// Nested generic: Box<Box<int>> in function signature
function acceptNestedBox(Box<Box<int>> $outer): int {
    return $outer->get()->get();
}

$inner = new Box<int>(42);
$outer = new Box<Box<int>>($inner);
echo acceptNestedBox($outer) . "\n";

// Multiple calls to nested — exercises cache with complex types
$inner2 = new Box<int>(99);
$outer2 = new Box<Box<int>>($inner2);
echo acceptNestedBox($outer2) . "\n";

// Generic param resolution does NOT use wrong cache (regression test for
// the cross-op_array cache_slot bug with strict_types + nested generics)
class Container<T> {
    public T $item;
    public function __construct(T $item) { $this->item = $item; }
    public function getItem(): T { return $this->item; }
}

$boxed = new Box<int>(7);
$cont = new Container<Box<int>>($boxed);
echo $cont->getItem()->get() . "\n";

// Verify property type enforcement with resolved generic params
try {
    $cont->item = "not a box";
} catch (TypeError $e) {
    echo "container: caught\n";
}

echo "OK\n";
?>
--EXPECT--
10
20
30
40
hello
rejected wrong type
99
100
101
42
99
7
container: caught
OK
