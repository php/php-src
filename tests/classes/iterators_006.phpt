--TEST--
ZE2 iterators and array wrapping
--FILE--
<?php

class ai implements Iterator {

    private $array;
    private $key;
    private $current;

    function __construct() {
        $this->array = array('foo', 'bar', 'baz');
    }

    function rewind(): void {
        reset($this->array);
        $this->next();
    }

    function valid(): bool {
        return $this->key !== NULL;
    }

    function key(): mixed {
        return $this->key;
    }

    function current(): mixed {
        return $this->current;
    }

    function next(): void {
        $this->key = key($this->array);
        $this->current = current($this->array);
        next($this->array);
    }
}

class a implements IteratorAggregate {

    public function getIterator(): Traversable {
        return new ai();
    }
}

$array = new a();

foreach ($array as $property => $value) {
    print "$property: $value\n";
}

#$array = $array->getIterator();
#$array->rewind();
#$array->valid();
#var_dump($array->key());
#var_dump($array->current());
echo "===2nd===\n";

$array = new ai();

foreach ($array as $property => $value) {
    print "$property: $value\n";
}

echo "===3rd===\n";

foreach ($array as $property => $value) {
    print "$property: $value\n";
}

?>
--EXPECT--
0: foo
1: bar
2: baz
===2nd===
0: foo
1: bar
2: baz
===3rd===
0: foo
1: bar
2: baz
