--TEST--
SPL: LimitIterator check limits are valid
--CREDITS--
Sean Burlington www.practicalweb.co.uk
TestFest London May 2009
--FILE--
<?php
  $array = array(array(7,8,9),1,2,3,array(4,5,6));
$arrayIterator = new ArrayIterator($array);

try {
  $limitIterator = new LimitIterator($arrayIterator, -1);
} catch (OutOfRangeException $e){
  print $e->getMessage(). "\n";
}


try {
  $limitIterator = new LimitIterator($arrayIterator, 0, -2);
} catch (OutOfRangeException $e){
  print $e->getMessage() . "\n";
}

try {
  $limitIterator = new LimitIterator($arrayIterator, 0, -1);
} catch (OutOfRangeException $e){
  print $e->getMessage() . "\n";
}



?>
===DONE===
--EXPECT--
Parameter offset must be >= 0
Parameter count must either be -1 or a value greater than or equal 0
===DONE===
