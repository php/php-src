--TEST--
list() with keys, evaluation order
--FILE--
<?php

// list($a => $b, $c => $d) = $e;
// Should be evaluated in the order:
// 1. Evaluate $e
// 2. Evaluate $a
// 3. Evaluate $e[$a]
// 4. Assign $b from $e[$a]
// 5. Evaluate $c
// 6. Evaluate $e[$c]
// 7. Assign $c from $e[$a]

// In order to observe this evaluation order, let's use some observer objects!

class Stringable
{
    private $name;
    public function __construct(string $name) {
        $this->name = $name;
    }

    public function __toString(): string {
        echo "$this->name evaluated.", PHP_EOL;
        return $this->name;
    }
}

class Indexable implements ArrayAccess
{
    public function offsetExists($offset): bool {
        echo "Existence of offset $offset checked for.", PHP_EOL;
        return true;
    }

    public function offsetUnset($offset): void {
        echo "Offset $offset removed.", PHP_EOL;
    }

    public function offsetGet($offset) {
        echo "Offset $offset retrieved.", PHP_EOL;
        return "value for offset $offset";
    }

    public function offsetSet($offset, $value): void {
        echo "Offset $offset set to $value.", PHP_EOL;
    }
}

class IndexableRetrievable
{
    private $label;
    private $indexable;
    
    public function __construct(string $label, Indexable $indexable) {
        $this->label = $label;
        $this->indexable = $indexable;
    }

    public function getIndexable(): Indexable {
        echo "Indexable $this->label retrieved.", PHP_EOL;
        return $this->indexable;
    }
}

$a = new Stringable("A");
$c = new Stringable("C");

$e = new IndexableRetrievable("E", new Indexable(["A" => "value for A", "C" => "value for C"]));

$store = new Indexable;

list((string)$a => $store["B"], (string)$c => $store["D"]) = $e->getIndexable();

?>
--EXPECT--
Indexable E retrieved.
A evaluated.
Offset A retrieved.
Offset B set to value for offset A.
C evaluated.
Offset C retrieved.
Offset D set to value for offset C.
