--TEST--
SPL: iterator_count() exceptions test
--CREDITS--
Lance Kesson jac_kesson@hotmail.com
#testfest London 2009-05-09
--FILE--
<?php
$array=array('a','b');

$iterator = new ArrayIterator($array);

iterator_count('1');

?>
--EXPECTF--
Fatal error: Uncaught TypeError: iterator_count(): Argument #1 ($iterator) must be of type Traversable|array, string given in %s:%d
Stack trace:
#0 %s(%d): iterator_count('1')
#1 {main}
  thrown in %s on line %d
