--TEST--
Generic class: opcache with inheritance and bound type arguments
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
declare(strict_types=1);

// Parent generic class
class Collection<T> {
    protected array $items = [];
    public function add(T $item): void { $this->items[] = $item; }
    public function first(): T { return $this->items[0]; }
    public function count(): int { return count($this->items); }
}

// Child with bound type
class IntCollection extends Collection<int> {}

// Grandchild adding new methods (not overriding)
class PositiveIntCollection extends IntCollection {
    public function addPositive(int $item): void {
        if ($item <= 0) throw new InvalidArgumentException("Must be positive");
        $this->add($item);
    }
}

// Generic child
class TypedCollection<T> extends Collection<T> {
    public function last(): T {
        return $this->items[count($this->items) - 1];
    }
}

// 1. Bound type child
$ic = new IntCollection();
$ic->add(1);
$ic->add(2);
echo "1. first: " . $ic->first() . ", count: " . $ic->count() . "\n";

try {
    $ic->add("bad");
} catch (TypeError $e) {
    echo "1. TypeError OK\n";
}

// 2. Grandchild
$pic = new PositiveIntCollection();
$pic->addPositive(5);
echo "2. first: " . $pic->first() . "\n";

try {
    $pic->addPositive(-1);
} catch (InvalidArgumentException $e) {
    echo "2. validation OK\n";
}

// 3. Generic child with explicit type
$tc = new TypedCollection<string>();
$tc->add("hello");
$tc->add("world");
echo "3. first: " . $tc->first() . ", last: " . $tc->last() . "\n";

try {
    $tc->add(42);
} catch (TypeError $e) {
    echo "3. TypeError OK\n";
}

// 4. Generic child with explicit type
$floats = new TypedCollection<float>();
$floats->add(3.14);
$floats->add(2.72);
echo "4. first: " . $floats->first() . ", last: " . $floats->last() . "\n";

echo "Done.\n";
?>
--EXPECT--
1. first: 1, count: 2
1. TypeError OK
2. first: 5
2. validation OK
3. first: hello, last: world
3. TypeError OK
4. first: 3.14, last: 2.72
Done.
