--TEST--
GH-16604 (Memory leaks in SPL constructors) - recursive iterators
--FILE--
<?php

$traversable = new RecursiveArrayIterator( [] );

$obj = new RecursiveIteratorIterator( $traversable );
$obj->__construct( $traversable );

$obj = new RecursiveTreeIterator( $traversable );
$obj->__construct( $traversable );

?>
--EXPECT--
