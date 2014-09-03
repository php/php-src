--TEST--
SPL: CachingInterator constructor flag checks 
--CREDITS--
Sean Burlington www.practicalweb.co.uk
TestFest London May 2009
--FILE--
<?php
  //line 681 ...
  $array = array(array(7,8,9),1,2,3,array(4,5,6));
$arrayIterator = new ArrayIterator($array);
try {
$test = new CachingIterator($arrayIterator, 0);
$test = new CachingIterator($arrayIterator, 1);
$test = new CachingIterator($arrayIterator, 2);
$test = new CachingIterator($arrayIterator, 3); // this throws an exception
} catch (InvalidArgumentException $e){
  print  $e->getMessage() . "\n";
}


?>
===DONE===
--EXPECTF--
Flags must contain only one of CALL_TOSTRING, TOSTRING_USE_KEY, TOSTRING_USE_CURRENT, TOSTRING_USE_INNER
===DONE===
