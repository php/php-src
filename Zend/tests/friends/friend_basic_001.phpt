--TEST--
A class can declare another class as a friend so that it may access protected and private properties
--FILE--
<?php

class Person
{
	friend PersonFormatter;

	private $firstName;
	private $lastName;

	public function __construct($firstName, $lastName)
	{
		$this->firstName = $firstName;
		$this->lastName = $lastName;
	}

	public function format()
	{
		return new PersonFormatter($this);
	}
}

class PersonFormatter
{
	private $person;

	public function __construct(Person $person)
	{
		$this->person = $person;
	}

	public function getFullName()
	{
		return $this->person->firstName . ' ' . $this->person->lastName;
	}
}

$person = new Person('Alice', 'Wonderland');
$formatter = $person->format();

var_dump($formatter->getFullName());
?>
--EXPECTF--
string(%d) "Alice Wonderland"