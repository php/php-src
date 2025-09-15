--TEST--
Typed property type coercion through ArrayIterator
--FILE--
<?php
class A implements IteratorAggregate {
    function __construct(
        public string $foo = 'bar'
    ) {}

    function getIterator(): Traversable {
        return new ArrayIterator($this);
    }
}

$obj = new A;
foreach ($obj as $k => &$v) {
	$v = 42;
}

var_dump($obj);
?>
--EXPECTF--
Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
object(A)#1 (1) {
  ["foo"]=>
  &string(2) "42"
}
