--TEST--
ZE2 ArrayAccess and sub Arrays
--FILE--
<?php 

class Peoples implements ArrayAccess {
	public $person;
	
	function __construct() {
		$this->person = array(array('name'=>'Joe'));
	}

	function offsetExists($index) {
		return array_key_exists($this->person, $index);
	}

	function &offsetGet($index) {
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
$people->person[0]['name'] = $people->person[0]['name'] . 'Foo';
var_dump($people->person[0]['name']);
$people->person[0]['name'] .= 'Bar';
var_dump($people->person[0]['name']);

echo "---ArrayOverloading---\n";

$people = new Peoples;

var_dump($people[0]['name']);
$people[0]['name'] = $people->person[0]['name'] . 'Foo';
var_dump($people[0]['name']);
$people[0]['name'] .= 'Bar';
var_dump($people[0]['name']);

echo "---Done---\n";
?>
--EXPECT--
string(3) "Joe"
string(6) "JoeFoo"
string(9) "JoeFooBar"
---ArrayOverloading---
string(3) "Joe"
string(6) "JoeFoo"
string(9) "JoeFooBar"
---Done---
