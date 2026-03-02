--TEST--
SPL: IteratorInterator constructor checks
--CREDITS--
Sean Burlington www.practicalweb.co.uk
TestFest London May 2009
--FILE--
<?php

$array = array(array(7,8,9),1,2,3,array(4,5,6));
$arrayIterator = new ArrayIterator($array);
try {
    $test = new IteratorIterator($arrayIterator);

    $test = new IteratorIterator($arrayIterator, 1);
    $test = new IteratorIterator($arrayIterator, 1, 1);
    $test = new IteratorIterator($arrayIterator, 1, 1, 1);
    $test = new IteratorIterator($arrayIterator, 1, 1, 1, 1);
} catch (TypeError $e){
  echo $e->getMessage() . "\n";
}

?>
--EXPECT--
IteratorIterator::__construct() expects at most 2 arguments, 3 given
