--TEST--
GH-19094 (Attaching class with no Iterator implementation to MultipleIterator causes crash)
--FILE--
<?php

class MyIterator implements Iterator {
    public function valid(): bool {
        return false;
    }

    public function current(): mixed {
        return null;
    }

    public function key(): string {
        return "";
    }

    public function next(): void {
    }

    public function rewind(): void {
    }
}

class MyAggregate implements IteratorAggregate {
    public function getIterator(): Traversable
    {
        throw new Error;
    }
}

$cls = new MultipleIterator();
$canary = new stdClass;
try {
    $cls[$canary] = 1;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $cls[new MyAggregate] = 1;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$cls[new MyIterator] = 1;
try {
    $cls->key();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Can only attach objects that implement the Iterator interface
Can only attach objects that implement the Iterator interface
Called key() with non valid sub iterator
