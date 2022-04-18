--TEST--
Bug #52238 - Crash when an Exception occurred in iterator_to_array
--FILE--
<?php
class Foo implements IteratorAggregate
{
    public function bar() {
        throw new Exception;
    }

    public function getIterator(): Traversable {
        return new ArrayIterator($this->bar());
    }
}
var_dump(iterator_to_array(new Foo));
?>
--EXPECTF--
Fatal error: Uncaught Exception in %s
Stack trace:
#0 %s: Foo->bar()
#1 [internal function]: Foo->getIterator()
#2 %s: iterator_to_array(Object(Foo))
#3 {main}
  thrown in %s on line %d
