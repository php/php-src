--TEST--
ZE2 ArrayAccess and ASSIGN_OP operators (.=)
--FILE--
<?php

class Peoples implements ArrayAccess {
	public $person;

	function __construct() {
		$this->person = array(array('name'=>'Foo'));
	}

	function offsetExists($index) {
		return array_key_exists($this->person, $index);
	}

	function offsetGet($index) {
		return $this->person[$index];
	}

	function offsetSet($index, $value) {
		$this->person[$index] = $value;
	}

	function offsetUnset($index) {
		unset($this->person[$index]);
	}
}

$people = new Peoples;

var_dump($people->person[0]['name']);
$people->person[0]['name'] = $people->person[0]['name'] . 'Bar';
var_dump($people->person[0]['name']);
$people->person[0]['name'] .= 'Baz';
var_dump($people->person[0]['name']);

echo "===ArrayOverloading===\n";

$people = new Peoples;

var_dump($people[0]['name']);
$people[0]['name'] = 'FooBar';
var_dump($people[0]['name']);
$people[0]['name'] = $people->person[0]['name'] . 'Bar';
var_dump($people[0]['name']);
$people[0]['name'] .= 'Baz';
var_dump($people[0]['name']);

?>
===DONE===
--EXPECTF--
string(3) "Foo"
string(6) "FooBar"
string(9) "FooBarBaz"
===ArrayOverloading===
string(3) "Foo"

Notice: Indirect modification of overloaded element of Peoples has no effect in %sarray_access_008.php on line 40
string(3) "Foo"

Notice: Indirect modification of overloaded element of Peoples has no effect in %sarray_access_008.php on line 42
string(3) "Foo"

Notice: Indirect modification of overloaded element of Peoples has no effect in %sarray_access_008.php on line 44
string(3) "Foo"
===DONE===
