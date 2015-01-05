--TEST--
SPL: RecursiveIteratorIterator, setMaxDepth check parameter count
--CREDITS--
Sean Burlington www.practicalweb.co.uk
TestFest London May 2009
--FILE--
<?php
  //line 681 ...
  $array = array(array(7,8,9),1,2,3,array(4,5,6));
$recursiveArrayIterator = new RecursiveArrayIterator($array);
$test = new RecursiveIteratorIterator($recursiveArrayIterator);

//var_dump($test->current());
$test->setMaxDepth();
$test->setMaxDepth(1);
$test->setMaxDepth(1,2);
$test->setMaxDepth(1,2,3);

//var_dump($test->current());


?>
===DONE===
--EXPECTF--
Warning: RecursiveIteratorIterator::setMaxDepth() expects at most 1 parameter, 2 given in %s on line 10

Warning: RecursiveIteratorIterator::setMaxDepth() expects at most 1 parameter, 3 given in %s on line 11
===DONE===
