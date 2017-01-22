--TEST--
SPL: iterator_to_array() exceptions test
--CREDITS--
Lance Kesson jac_kesson@hotmail.com
#testfest London 2009-05-09
--FILE--
<?php
$array=array('a','b');

$iterator = new ArrayIterator($array);

iterator_to_array();


iterator_to_array($iterator,'test','test');

iterator_to_array('test','test');

?>
--EXPECTF--
Warning: iterator_to_array() expects at least 1 parameter, 0 given in %s

Warning: iterator_to_array() expects at most 2 parameters, 3 given in %s

Fatal error: Uncaught TypeError: Argument 1 passed to iterator_to_array() must implement interface Traversable, string given in %s:%d
Stack trace:
#0 %s(%d): iterator_to_array('test', 'test')
#1 {main}
  thrown in %s on line %d
