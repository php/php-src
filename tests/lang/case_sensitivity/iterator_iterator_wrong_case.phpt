--TEST--
IteratorIterator and RecursiveIteratorIterator class cast with wrong case fails with wrong case
--FILE--
<?php
class MyAggregate implements IteratorAggregate {
    public function getIterator(): ArrayIterator {
        return new ArrayIterator([1, 2, 3]);
    }
}

$it = new IteratorIterator(new MyAggregate(), "MYAGGREGATE");
echo "ok\n";
?>
--EXPECTF--
Fatal error: Uncaught LogicException: Class to downcast to not found or not base class or does not implement Traversable in %s:%d
Stack trace:
#0 %s(8): IteratorIterator->__construct(Object(MyAggregate), 'MYAGGREGATE')
#1 {main}
  thrown in %s on line %d
