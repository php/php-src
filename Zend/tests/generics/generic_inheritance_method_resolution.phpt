--TEST--
Generic class: method signature resolution in inheritance
--FILE--
<?php
class Collection<T> {
    private array $items = [];
    public function add(T $item): void { $this->items[] = $item; }
    public function first(): T { return $this->items[0]; }
}

// Override with resolved concrete types — should be compatible
class IntList extends Collection<int> {
    public function add(int $item): void {
        echo "IntList::add($item)\n";
        parent::add($item);
    }
    public function first(): int {
        return parent::first();
    }
}

$list = new IntList();
$list->add(42);
echo $list->first() . "\n";
echo "OK\n";
?>
--EXPECT--
IntList::add(42)
42
OK
