--TEST--
TextIterator::next(): Testing with no data
--FILE--
<?php

$x = new textiterator;
var_dump($x->next());

?>
--EXPECTF--
Warning: TextIterator::__construct() expects at least 1 parameter, 0 given in %s on line %d
int(-1)
