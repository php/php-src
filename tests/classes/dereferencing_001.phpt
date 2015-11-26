--TEST--
ZE2 dereferencing of objects from methods
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class Name {
	function __construct($_name) {
		$this->name = $_name;
	}

	function display() {
		echo $this->name . "\n";
	}
}

class Person {
	private $name;

	function __construct($_name, $_address) {
		$this->name = new Name($_name);
	}

	function getName() {
		return $this->name;
	}
}

$person = new Person("John", "New York");
$person->getName()->display();

?>
--EXPECT--
John
