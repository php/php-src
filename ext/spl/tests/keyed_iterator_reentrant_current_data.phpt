--TEST--
Keyed iterator consumers retain the current value across reentrant key() calls
--FILE--
<?php

class ReentrantKeyIterator implements RecursiveIterator {
    private int $index = 0;

    public function __construct(
        private array $values,
        private bool $leaf = false,
    ) {}

    public function rewind(): void {
        $this->index = 0;
    }

    public function valid(): bool {
        return $this->index < count($this->values);
    }

    public function current(): mixed {
        return $this->values[$this->index];
    }

    public function key(): mixed {
        global $iterator, $advanced, $spraySource, $holder;

        $key = $this->index;
        if ($this->leaf && !$advanced) {
            $advanced = true;
            $iterator->next();

            $holder = new IteratorIterator($spraySource);
            $holder->rewind();
            $holder->current();
        }

        return $key;
    }

    public function next(): void {
        ++$this->index;
    }

    public function hasChildren(): bool {
        return !$this->leaf;
    }

    public function getChildren(): ?RecursiveIterator {
        return $this->values[$this->index];
    }
}

function createIterator(): RecursiveIteratorIterator {
    global $iterator, $advanced, $spraySource, $holder;

    $advanced = false;
    $holder = null;
    $spraySource = new ReentrantKeyIterator(['replacement'], true);
    $leaf = new ReentrantKeyIterator(['original'], true);
    $root = new ReentrantKeyIterator([$leaf]);
    return $iterator = new RecursiveIteratorIterator($root);
}

var_dump(iterator_to_array(createIterator(), true));

foreach (createIterator() as $key => $value) {
    var_dump($key, $value);
}

$target = new stdClass();
foreach (createIterator() as $key => $target->value) {
    var_dump($key, $target->value);
}

?>
--EXPECT--
array(1) {
  [0]=>
  string(8) "original"
}
int(0)
string(8) "original"
int(0)
string(8) "original"
