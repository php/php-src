--TEST--
Readonly property modification error through ArrayIterator
--FILE--
<?php
class A implements IteratorAggregate {
    function __construct(
        public readonly string $foo = 'bar'
    ) {}

    function getIterator(): Traversable {
        return new ArrayIterator($this);
    }
}

$obj = new A;

try {
    foreach ($obj as $k => &$v) {}
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

var_dump($obj);
?>
--EXPECTF--
Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
Cannot acquire reference to readonly property A::$foo
object(A)#1 (1) {
  ["foo"]=>
  string(3) "bar"
}
