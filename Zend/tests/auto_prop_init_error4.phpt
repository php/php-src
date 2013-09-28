--TEST--
Automatic property initialization: Cannot be used on abstract methods
--FILE--
<?php

class Test {
    abstract public function __construct($this->bar);
}

?>
--EXPECTF--
Fatal error: Automatic property initialization can not be used in an abstract method in %s on line %d
