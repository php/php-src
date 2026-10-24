--TEST--
GH-21927: Use-after-free of self-freeing MultipleIterator children
--FILE--
<?php

class It extends ArrayIterator {
    public $mi;
    public function valid(): bool { $this->mi->detachIterator($this); return true; }
}
$mi = new MultipleIterator();
$it = new It([1, 2]);
$it->mi = $mi;
$mi->attachIterator($it);
unset($it);
var_dump($mi->current());

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
