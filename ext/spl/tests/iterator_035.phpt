--TEST--
SPL: ArrayIterator and values assigned by reference
--FILE--
<?php

$tmp = 1;

$a = new ArrayIterator();
$a[] = $tmp;
var_dump($a);
try {
    $a[] = &$tmp;
    var_dump($a);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
object(ArrayIterator)#1 (1) {
  ["storage":"ArrayIterator":private]=>
  array(1) {
    [0]=>
    int(1)
  }
}
Error: Cannot assign by reference to an array dimension of an object
Done
