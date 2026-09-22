--TEST--
Bug #42703 (Exception raised in an iterator::current() causes segfault in FilterIterator)
--FILE--
<?php
class BlaIterator implements Iterator
{
    public function rewind(): void { }

    public function next(): void { }

    public function valid(): bool {
        return true;
    }

    public function current(): mixed
    {
      throw new Exception('boo');
    }

    public function key(): mixed { return null; }
}

$it = new BlaIterator();
$itit = new IteratorIterator($it);

try {
  foreach($itit as $key => $value) {
    echo $key, $value;
  }
}
catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($itit->current());
var_dump($itit->key());
?>
--EXPECT--
Exception: boo
NULL
NULL
