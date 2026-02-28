--TEST--
Generic class: constrained type parameter with upper bound
--FILE--
<?php
class TypedCollection<T: Countable> {
    private array $items = [];
    public function add(T $item): void {
        $this->items[] = $item;
    }
    public function count(): int {
        return count($this->items);
    }
}

echo "TypedCollection declared\n";

$c = new TypedCollection();
echo "Instantiated\n";
?>
--EXPECT--
TypedCollection declared
Instantiated
