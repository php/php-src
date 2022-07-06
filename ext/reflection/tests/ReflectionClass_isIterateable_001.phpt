--TEST--
ReflectionClass::isIterateable()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
Interface ExtendsIterator extends Iterator {
}
Interface ExtendsIteratorAggregate extends IteratorAggregate {
}
Class IteratorImpl implements Iterator {
    public function next() {}
    public function key() {}
    public function rewind() {}
    public function current() {}
    public function valid() {}
}
Class IteratorAggregateImpl implements IteratorAggregate {
    public function getIterator() {}
}
Class ExtendsIteratorImpl extends IteratorImpl {
}
Class ExtendsIteratorAggregateImpl extends IteratorAggregateImpl {
}
Class A {
}

$classes = array('Traversable', 'Iterator', 'IteratorAggregate', 'ExtendsIterator', 'ExtendsIteratorAggregate',
      'IteratorImpl', 'IteratorAggregateImpl', 'ExtendsIteratorImpl', 'ExtendsIteratorAggregateImpl', 'A');

foreach($classes as $class) {
    $rc = new ReflectionClass($class);
    echo "Is $class iterable? ";
    var_dump($rc->isIterateable());
}

echo "\nTest static invocation:\n";
ReflectionClass::isIterateable();

?>
--EXPECTF--
Is Traversable iterable? bool(false)
Is Iterator iterable? bool(false)
Is IteratorAggregate iterable? bool(false)
Is ExtendsIterator iterable? bool(false)
Is ExtendsIteratorAggregate iterable? bool(false)
Is IteratorImpl iterable? bool(true)
Is IteratorAggregateImpl iterable? bool(true)
Is ExtendsIteratorImpl iterable? bool(true)
Is ExtendsIteratorAggregateImpl iterable? bool(true)
Is A iterable? bool(false)

Test static invocation:

Fatal error: Uncaught Error: Non-static method ReflectionClass::isIterateable() cannot be called statically in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
