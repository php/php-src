--TEST--
Generic class: constraint enforcement at instantiation
--FILE--
<?php
class TypedCollection<T: Countable> {
    private array $items = [];
    public function add(T $item): void {
        $this->items[] = $item;
    }
}

// ArrayObject implements Countable — should work
$c = new TypedCollection<ArrayObject>();
echo "ArrayObject accepted\n";

// string does NOT implement Countable — should fatal error
$c2 = new TypedCollection<string>();
echo "Should not reach here\n";
?>
--EXPECTF--
ArrayObject accepted

Fatal error: Generic type argument #1 must implement Countable, string given in %s on line %d
