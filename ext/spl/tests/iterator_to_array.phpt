--TEST--
SPL: iterator_to_array() exceptions test
--CREDITS--
Lance Kesson jac_kesson@hotmail.com
#testfest London 2009-05-09
--FILE--
<?php
$array=array('a','b');

$iterator = new ArrayIterator($array);

iterator_to_array('test','test');

?>
--EXPECTF--
Fatal error: Uncaught TypeError: iterator_to_array(): Argument #1 ($iterator) must be of type Traversable, string given in %s:%d
Stack trace:
#0 %s(%d): iterator_to_array('test', 'test')
#1 {main}
  thrown in %s on line %d
