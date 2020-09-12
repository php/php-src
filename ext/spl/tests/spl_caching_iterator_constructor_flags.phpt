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
new CachingIterator($arrayIterator, 0); /* TODO Should this throw? */
new CachingIterator($arrayIterator, CachingIterator::CALL_TOSTRING);
new CachingIterator($arrayIterator, CachingIterator::TOSTRING_USE_KEY);
new CachingIterator($arrayIterator, CachingIterator::TOSTRING_USE_CURRENT);
new CachingIterator($arrayIterator, CachingIterator::TOSTRING_USE_INNER);
try {
    $test = new CachingIterator($arrayIterator, 3); // this throws an exception
} catch (\ValueError $e){
  print  $e->getMessage() . "\n";
}


?>
--EXPECT--
CachingIterator::__construct(): Argument #2 ($flags) must contain only one of CachingIterator::CALL_TOSTRING, CachingIterator::TOSTRING_USE_KEY, CachingIterator::TOSTRING_USE_CURRENT, or CachingIterator::TOSTRING_USE_INNER
