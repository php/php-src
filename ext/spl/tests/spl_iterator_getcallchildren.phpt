--TEST--
SPL: RecursiveIteratorIterator, getCallChildren
--CREDITS--
Sean Burlington www.practicalweb.co.uk
TestFest London May 2009
--FILE--
<?php
  //line 681 ...
  $array = array(array(7,8,9),1,2,3,array(4,5,6));
$recursiveArrayIterator = new RecursiveArrayIterator($array);
$test = new RecursiveIteratorIterator($recursiveArrayIterator);

var_dump($test->current());
$test->next();
var_dump($test->current());
try {
  $output = $test->callGetChildren();
} catch (TypeError $exception) {
  $output = null;
  echo $exception->getMessage() . "\n";
}
var_dump($output);


?>
--EXPECT--
array(3) {
  [0]=>
  int(7)
  [1]=>
  int(8)
  [2]=>
  int(9)
}
int(7)
ArrayIterator::__construct(): Argument #1 ($array) must be of type array, int given
NULL
