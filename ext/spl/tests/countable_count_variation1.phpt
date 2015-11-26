--TEST--
SPL: Countable::count() with wrong return types and exception.
--FILE--
<?php

Class returnNull implements Countable {
	function count() {
	}
}

Class returnString implements Countable {
	function count() {
		return "hello";
	}
}

Class returnObject implements Countable {
	function count() {
		return new returnObject;
	}
}

Class returnArray implements Countable {
	function count() {
		return array(1,2,3);
	}
}

Class throwException implements Countable {
	function count() {
		throw new Exception('Thrown from count');
	}
}


echo "Count returns null:\n";
var_dump(count(new returnNull));

echo "Count returns a string:\n";
var_dump(count(new returnString));

echo "Count returns an object:\n";
var_dump(count(new returnObject));

echo "Count returns an array:\n";
var_dump(count(new returnArray));

echo "Count throws an exception:\n";
try {
	echo count(new throwException);
} catch (Exception $e) {
	echo $e->getMessage();
}

?>
--EXPECTF--
Count returns null:
int(0)
Count returns a string:
int(0)
Count returns an object:

Notice: Object of class returnObject could not be converted to int in %s on line 40
int(1)
Count returns an array:
int(1)
Count throws an exception:
Thrown from count