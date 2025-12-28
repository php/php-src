--TEST--
GH-19577: Integer overflow in LimitIterator with small offset and PHP_INT_MAX count
--FILE--
<?php

$it = new ArrayIterator(array(0 => 'A', 1 => 'B', 2 => 'C', 3 => 'D'));
$it = new LimitIterator($it, 2, PHP_INT_MAX);

foreach($it as $val => $key) {
    echo "Key: $val, Value: $key\n";
}

?>
--EXPECT--
Key: 2, Value: C
Key: 3, Value: D
