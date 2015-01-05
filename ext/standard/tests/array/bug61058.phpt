--TEST--
Bug #61058 (array_fill leaks if start index is PHP_INT_MAX)
--FILE--
<?php 
array_fill(PHP_INT_MAX, 2, '*');
?>
--EXPECTF--
Warning: array_fill(): Cannot add element to the array as the next element is already occupied in %sbug61058.php on line %d
