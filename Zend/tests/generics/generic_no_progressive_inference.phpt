--TEST--
Generic class: no progressive inference — types are fixed at construction
--DESCRIPTION--
Verifies that generic type parameters are determined at construction time and
remain fixed. There is no progressive/flow-sensitive inference from method calls.

Three scenarios:
1. Explicit type args: type is fixed, subsequent calls are checked
2. Constructor inference: type is inferred from ctor args, then fixed
3. No inference possible: type is unconstrained (acts as mixed)

Also tests the variance scenario: Box(new Child1) produces Box<Child1>,
which correctly rejects Child2 assignment and Box<Elder> parameter hints.
--FILE--
<?php
declare(strict_types=1);

class Collection<T> {
    private array $items = [];
    public function add(T $item): void { $this->items[] = $item; }
    public function getAll(): array { return $this->items; }
}

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function set(T $value): void { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// === 1. Explicit type args: fixed at construction ===
echo "--- Explicit type args ---\n";
$c = new Collection<int>();
$c->add(1);
$c->add(2);
echo "Added ints: OK\n";

try {
    $c->add("text");
} catch (TypeError $e) {
    echo "Rejected string in Collection<int>: OK\n";
}

// === 2. Constructor inference: fixed after inference ===
echo "--- Constructor inference ---\n";
$box = new Box(42);  // infers T=int
echo "Inferred Box<int>: " . $box->get() . "\n";

try {
    $box->set("hello");
} catch (TypeError $e) {
    echo "Rejected string in inferred Box<int>: OK\n";
}

$sbox = new Box("hello");  // infers T=string
echo "Inferred Box<string>: " . $sbox->get() . "\n";

try {
    $sbox->set([1, 2, 3]);
} catch (TypeError $e) {
    echo "Rejected array in inferred Box<string>: OK\n";
}

// === 3. No inference possible: unconstrained (acts as mixed) ===
echo "--- No inference possible ---\n";
$uc = new Collection();
$uc->add(1);
$uc->add("text");
$uc->add(3.14);
$uc->add(true);
$uc->add(null);
$uc->add([1, 2]);
echo "Unconstrained accepts all types: OK\n";
echo "Count: " . count($uc->getAll()) . "\n";

// === 4. Variance: inferred type prevents subtype mixing ===
echo "--- Variance with inheritance ---\n";

class Elder {}
class Child1 extends Elder {}
class Child2 extends Elder {}

$b = new Box(new Child1());  // infers T=Child1

// Child2 is not Child1, should be rejected
try {
    $b->value = new Child2();
} catch (TypeError $e) {
    echo "Rejected Child2 in Box<Child1>: OK\n";
}

// Another Child1 should be accepted
$b->value = new Child1();
echo "Accepted Child1 in Box<Child1>: OK\n";

// === 5. Explicit Box<Elder> accepts both children ===
echo "--- Explicit wider type ---\n";
$eb = new Box<Elder>(new Child1());
$eb->value = new Child2();
echo "Box<Elder> accepts Child2: OK\n";
$eb->value = new Child1();
echo "Box<Elder> accepts Child1: OK\n";

// === 6. Box<Child1> does not satisfy Box<Elder> (invariant) ===
echo "--- Invariant type parameter ---\n";

function takesElderBox(Box<Elder> $b): string {
    return get_class($b->value);
}

try {
    takesElderBox(new Box(new Child1()));
} catch (TypeError $e) {
    echo "Box<Child1> rejected for Box<Elder> param: OK\n";
}

// But Box<Elder> works
echo "Box<Elder> accepted: " . takesElderBox(new Box<Elder>(new Child1())) . "\n";

echo "Done.\n";
?>
--EXPECT--
--- Explicit type args ---
Added ints: OK
Rejected string in Collection<int>: OK
--- Constructor inference ---
Inferred Box<int>: 42
Rejected string in inferred Box<int>: OK
Inferred Box<string>: hello
Rejected array in inferred Box<string>: OK
--- No inference possible ---
Unconstrained accepts all types: OK
Count: 6
--- Variance with inheritance ---
Rejected Child2 in Box<Child1>: OK
Accepted Child1 in Box<Child1>: OK
--- Explicit wider type ---
Box<Elder> accepts Child2: OK
Box<Elder> accepts Child1: OK
--- Invariant type parameter ---
Box<Child1> rejected for Box<Elder> param: OK
Box<Elder> accepted: Child1
Done.
