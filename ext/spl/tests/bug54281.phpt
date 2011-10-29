--TEST--
Bug #54281 (Crash in spl_recursive_it_rewind_ex)
--FILE--
<?php

class RecursiveArrayIteratorIterator extends RecursiveIteratorIterator {
	function __construct($it, $max_depth) { }
}
$it = new RecursiveArrayIteratorIterator(new RecursiveArrayIterator(array()), 2);

foreach($it as $k=>$v) { }

?>
--EXPECTF--
Fatal error: RecursiveIteratorIterator::rewind(): The RecursiveArrayIteratorIterator instance wasn't initialized properly in %s on line %d
