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
} catch (Error $e) {
	echo $e->getMessage(), "\n";
}

?>
===DONE===
--EXPECT--
Object of class stdClass could not be converted to string
===DONE===
