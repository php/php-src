--TEST--
GH-22049: ArrayIterator clone after malformed unserialize should not assert
--FILE--
<?php

$payload = 'O:13:"ArrayIterator":3:{'
    . 's:1:"0";i:0;'
    . 's:1:"1";a:0:{}'
    . 's:1:"2";a:2:{i:0;i:0;i:1;i:0;}'
    . '}';

$obj = unserialize($payload);

$clone = clone $obj;

echo "OK\n";
?>
--EXPECTF--
Deprecated: Creation of dynamic property ArrayIterator::$0 is deprecated in %s on line %d

Deprecated: Creation of dynamic property ArrayIterator::$1 is deprecated in %s on line %d
OK