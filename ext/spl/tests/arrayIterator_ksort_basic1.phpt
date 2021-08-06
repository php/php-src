--TEST--
Bug #79653: Unexpected error for ArrayIterator::ksort()
--FILE--
<?php

$array = [3 => 1, 2 => 2, 1 => 3];
$arrIter = new ArrayIterator($array);
var_dump($arrIter->ksort());
var_dump($arrIter);

?>
--EXPECTF--
Deprecated: Method ArrayIterator::ksort() is deprecated in %s on line %d
bool(true)
object(ArrayIterator)#%d (1) {
  ["storage":"ArrayIterator":private]=>
  array(3) {
    [1]=>
    int(3)
    [2]=>
    int(2)
    [3]=>
    int(1)
  }
}
