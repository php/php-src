--TEST--
SPL: RecursiveTreeIterator and Exception from getEntry()
--FILE--
<?php

$ary = [
    ['string'],
    [new stdClass],
];

class RecursiveArrayIteratorAggregated implements IteratorAggregate {
    public RecursiveArrayIterator $it;
    function __construct(array $it) {
        $this->it = new RecursiveArrayIterator($it);
    }
    function getIterator(): Traversable {
        return $this->it;
    }
}

$it = new RecursiveArrayIteratorAggregated($ary);
try {
    foreach(new RecursiveTreeIterator($it) as $k => $v) {
        echo "[$k] => $v\n";
    }
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
[0] => |-Array
[0] => | \-string
[1] => \-Array

Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
Object of class stdClass could not be converted to string
