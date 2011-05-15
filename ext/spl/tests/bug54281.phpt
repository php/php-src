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
Fatal error: Uncaught exception 'LogicException' with message 'In the constructor of RecursiveArrayIteratorIterator, parent::__construct() must be called and its exceptions cannot be cleared' in %s:%d
Stack trace:
#0 %s(%d): RecursiveArrayIteratorIterator->internal_construction_wrapper(Object(RecursiveArrayIterator), 2)
#1 {main}
  thrown in %s on line %d
