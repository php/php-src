--TEST--
foreach with iterator and &$value reference
--FILE--
<?php

class MyIterator implements Iterator {
	public function valid() { return true; }
	public function next() {	}
	public function rewind() {	}
	public function current() {	}
	public function key() {	}
}

$f = new MyIterator;
echo "-----( Try to iterate with &\$value: )-----\n";
foreach ($f as $k=>&$v) {
	echo "$k => $v\n";	
}

?>
--EXPECTF--
-----( Try to iterate with &$value: )-----

Fatal error: Uncaught Error: An iterator cannot be used with foreach by reference in %s:13
Stack trace:
#0 {main}
  thrown in %s on line 13
