--TEST--
Generic class: multi-level inheritance with bound type arguments
--FILE--
<?php
declare(strict_types=1);

// A<T> -> B<T> extends A<T> -> C extends B<int>
class Collection<T> {
    protected array $items = [];
    public function add(T $item): void { $this->items[] = $item; }
    public function first(): T { return $this->items[0]; }
    public function count(): int { return count($this->items); }
}

class SortableCollection<T> extends Collection<T> {
    public function sort(): void { sort($this->items); }
}

class IntSortableCollection extends SortableCollection<int> {}

// 1. Three-level chain works
$c = new IntSortableCollection();
$c->add(3);
$c->add(1);
$c->add(2);
$c->sort();
echo "1. first: " . $c->first() . ", count: " . $c->count() . "\n";

// 2. Type enforcement at bottom level
try {
    $c->add("not an int");
} catch (TypeError $e) {
    echo "2. TypeError OK\n";
}

// 3. Middle level with explicit args
$sc = new SortableCollection<string>();
$sc->add("banana");
$sc->add("apple");
$sc->sort();
echo "3. first: " . $sc->first() . "\n";

try {
    $sc->add(42);
} catch (TypeError $e) {
    echo "3. TypeError OK\n";
}

// 4. Direct instantiation with explicit type
$typed = new Collection<float>();
$typed->add(1.1);
$typed->add(2.2);
echo "4. first: " . $typed->first() . ", count: " . $typed->count() . "\n";

try {
    $typed->add("bad");
} catch (TypeError $e) {
    echo "4. TypeError OK\n";
}

echo "Done.\n";
?>
--EXPECT--
1. first: 1, count: 3
2. TypeError OK
3. first: apple
3. TypeError OK
4. first: 1.1, count: 2
4. TypeError OK
Done.
