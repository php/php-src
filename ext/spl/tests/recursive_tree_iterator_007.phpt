--TEST--
SPL: RecursiveTreeIterator and Exception from getEntry()
--INI--
error_reporting=E_ALL&~E_NOTICE
--FILE--
<?php

$ary = array(new stdClass);

class RecursiveArrayIteratorAggregated implements IteratorAggregate {
	public $it;
	function __construct($it) {
		$this->it = new RecursiveArrayIterator($it);
	}
	function getIterator() {
		return $this->it;
	}
}

$it = new RecursiveArrayIteratorAggregated($ary);
try {
	foreach(new RecursiveTreeIterator($it) as $k => $v) {
		echo "[$k] => $v\n";
	}
} catch (UnexpectedValueException $e) {
	echo "UnexpectedValueException thrown\n";
}

?>
===DONE===
--EXPECTF--
UnexpectedValueException thrown
===DONE===
