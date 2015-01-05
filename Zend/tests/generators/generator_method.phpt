--TEST--
Methods can be generators
--FILE--
<?php

class Test implements IteratorAggregate {
    protected $data;

    public function __construct(array $data) {
        $this->data = $data;
    }

    public function getIterator() {
        foreach ($this->data as $value) {
            yield $value;
        }
    }
}

$test = new Test(['foo', 'bar', 'baz']);
foreach ($test as $value) {
    var_dump($value);
}

?>
--EXPECT--
string(3) "foo"
string(3) "bar"
string(3) "baz"
