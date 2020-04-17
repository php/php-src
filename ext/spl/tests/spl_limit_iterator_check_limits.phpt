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
--EXPECT--
LimitIterator::__construct(): Argument #1 ($iterator) must be greater or equal to 0
LimitIterator::__construct(): Argument #2 ($offset) must be greater or equal to 0
object(LimitIterator)#3 (0) {
}
