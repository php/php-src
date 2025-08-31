--TEST--
yield from with an IteratorAggregate
--FILE--
<?php
class foo implements \IteratorAggregate {
  public $prop = 1;
  function getIterator(): Traversable {
    var_dump($this->prop);
    yield;
  }
}
(function(){
  yield from new foo;
})()->next();
?>
--EXPECT--
int(1)
