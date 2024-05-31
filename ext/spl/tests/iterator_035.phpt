--TEST--
SPL: ArrayIterator and values assigned by reference
--FILE--
<?php

$tmp = 1;

$a = new ArrayIterator();
$a[] = $tmp;
$a[] = &$tmp;

var_dump($a);
echo "Done\n";
?>
--EXPECT--
object(ArrayIterator)#1 (1) {
  ["storage":"ArrayIterator":private]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    &int(1)
  }
}
Done
