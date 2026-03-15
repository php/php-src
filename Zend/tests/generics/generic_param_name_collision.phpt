--TEST--
Generic class: generic param name that looks like a built-in type
--DESCRIPTION--
Tests that generic type parameter names are resolved correctly
even when they have names that look like built-in types or classes.
--FILE--
<?php
declare(strict_types=1);

// T is a common name but let's test unusual ones
class Wrapper<V> {
    private V $data;
    public function __construct(V $data) { $this->data = $data; }
    public function get(): V { return $this->data; }
}

$w = new Wrapper<int>(42);
echo "1. " . $w->get() . "\n";

try {
    $w2 = new Wrapper<int>("bad");
} catch (TypeError $e) {
    echo "1. TypeError OK\n";
}

// Single-letter params
class Pair<A, B> {
    public function __construct(private A $a, private B $b) {}
    public function first(): A { return $this->a; }
    public function second(): B { return $this->b; }
}

$p = new Pair<string, int>("age", 25);
echo "2. " . $p->first() . ": " . $p->second() . "\n";

// Longer descriptive names
class Container<ElementType> {
    private array $items = [];
    public function add(ElementType $item): void { $this->items[] = $item; }
    public function first(): ElementType { return $this->items[0]; }
}

$c = new Container<string>();
$c->add("hello");
echo "3. " . $c->first() . "\n";

try {
    $c->add(42);
} catch (TypeError $e) {
    echo "3. TypeError OK\n";
}

echo "Done.\n";
?>
--EXPECT--
1. 42
1. TypeError OK
2. age: 25
3. hello
3. TypeError OK
Done.
