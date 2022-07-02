--TEST--
SPL: FixedArray: overriding getIterator()
--FILE--
<?php
class A extends SplFixedArray
{
  public function getIterator(): Iterator
  {
    $iterator = parent::getIterator();
    while ($iterator->valid()) {
      echo "In A: key={$iterator->key()} value={$iterator->current()}\n";
      yield $iterator->key() => $iterator->current();
      $iterator->next();
    }
  }
}

echo "==SplFixedArray instance==\n";
$a = new SplFixedArray(3);
$a[0] = "a";
$a[1] = "b";
$a[2] = "c";
foreach ($a as $k => $v) {
  echo "$k => $v\n";
}

echo "==Subclass instance==\n";
$a = new A(3);
$a[0] = "d";
$a[1] = "e";
$a[2] = "f";
foreach ($a as $k => $v) {
  echo "$k => $v\n";
}
--EXPECT--
==SplFixedArray instance==
0 => a
1 => b
2 => c
==Subclass instance==
In A: key=0 value=d
0 => d
In A: key=1 value=e
1 => e
In A: key=2 value=f
2 => f
