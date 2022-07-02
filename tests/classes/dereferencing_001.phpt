--TEST--
ZE2 dereferencing of objects from methods
--FILE--
<?php

class Name {
    function __construct(public $name) {}

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
