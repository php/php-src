--TEST--
Generic class: inline cache with union type args reduces O(nm) to O(1) via interning
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// Union type inside generic args: Box<int|string>
// Without interning, checking Box<int|string> against Box<int|string> is O(nm)
// because each union member must be compared against each expected member.
// With interning + pointer equality, this becomes O(1).

function acceptBoxIntOrString(Box<int|string> $box): int|string {
    return $box->get();
}

// Explicit instantiation with union type
$b1 = new Box<int|string>(42);
echo acceptBoxIntOrString($b1) . "\n";

// Repeated calls — all should hit the monomorphic pointer-equality fast path
$b2 = new Box<int|string>("hello");
echo acceptBoxIntOrString($b2) . "\n";

$b3 = new Box<int|string>(100);
echo acceptBoxIntOrString($b3) . "\n";

$b4 = new Box<int|string>("world");
echo acceptBoxIntOrString($b4) . "\n";

// Box<int> is a subtype of Box<int|string> — int ⊆ int|string
$b5 = new Box<int>(99);
echo acceptBoxIntOrString($b5) . "\n";

// Inferred Box<int> should also be accepted
$b6 = new Box(77);
echo acceptBoxIntOrString($b6) . "\n";

// Box<string> is also a subtype of Box<int|string>
$b7 = new Box<string>("sub");
echo acceptBoxIntOrString($b7) . "\n";

// Wrong type should still be rejected — float is NOT in int|string
try {
    acceptBoxIntOrString(new Box<float>(3.14));
} catch (TypeError $e) {
    echo "rejected float box\n";
}

// Larger union: Box<int|string|float>
function acceptBoxNumOrString(Box<int|string|float> $box): int|string|float {
    return $box->get();
}

$n1 = new Box<int|string|float>(1);
$n2 = new Box<int|string|float>("two");
$n3 = new Box<int|string|float>(3.0);
echo acceptBoxNumOrString($n1) . "\n";
echo acceptBoxNumOrString($n2) . "\n";
echo acceptBoxNumOrString($n3) . "\n";

// Repeated calls to exercise cache
for ($i = 0; $i < 10; $i++) {
    $tmp = new Box<int|string|float>($i);
    acceptBoxNumOrString($tmp);
}
echo "10 cached calls OK\n";

// Multi-param with union types
class Pair<T, U> {
    public function __construct(public T $first, public U $second) {}
}

function acceptPairMixed(Pair<int|string, float|bool> $p): string {
    return $p->first . ":" . ($p->second ? "true" : "false");
}

$p1 = new Pair<int|string, float|bool>(42, true);
echo acceptPairMixed($p1) . "\n";

$p2 = new Pair<int|string, float|bool>("hello", false);
echo acceptPairMixed($p2) . "\n";

// Repeated calls — same interned args
$p3 = new Pair<int|string, float|bool>(99, true);
echo acceptPairMixed($p3) . "\n";

// Wrong union type rejected
try {
    acceptPairMixed(new Pair<int, int>(1, 2));
} catch (TypeError $e) {
    echo "rejected wrong pair type\n";
}

echo "OK\n";
?>
--EXPECT--
42
hello
100
world
99
77
sub
rejected float box
1
two
3
10 cached calls OK
42:true
hello:false
99:true
rejected wrong pair type
OK
