--TEST--
Explicit iterator implementation
--FILE--
<?php

class Foo implements Iterator {
    public string $hook {
        get => 'this is not the correct value';
    }

    private $x = ['foo', 'BAR'];
    private $cursor = 0;

    public function current(): string { return $this->x[$this->cursor]; }
    public function key(): int { return $this->cursor; }
    public function next(): void { ++$this->cursor; }
    public function rewind(): void { $this->cursor = 0; }
    public function valid(): bool { return isset($this->x[$this->cursor]); }
}

class Bar implements IteratorAggregate {
    public string $hook {
        get => 'this is not the correct value';
    }

    public function getIterator(): Traversable {
        yield 1;
        yield 2;
    }
}

var_dump(iterator_to_array(new Foo()));
var_dump(iterator_to_array(new Bar()));

?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "BAR"
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
