--TEST--
SPL: LimitIterator seek() arguments
--CREDITS--
Roshan Abraham (roshanabrahams@gmail.com)
TestFest London May 2009
--FILE--
<?php

$a = array(1,2,3);
$lt = new LimitIterator(new ArrayIterator($a));

$lt->seek(1,1); // Should throw a warning as seek expects only 1 argument

?>
--EXPECTF--

Warning: LimitIterator::seek() expects exactly 1 parameter, 2 given in %s on line %d

