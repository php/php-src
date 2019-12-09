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
    var_dump(new LimitIterator($arrayIterator, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    var_dump(new LimitIterator($arrayIterator, 0, -2));
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

var_dump(new LimitIterator($arrayIterator, 0, -1));

?>
--EXPECTF--
Parameter offset must be >= 0
Parameter count must either be -1 or a value greater than or equal 0
object(LimitIterator)#%d (%d) {
}