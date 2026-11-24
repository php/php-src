--TEST--
Non-readonly abstract class cannot implement Operable
--FILE--
<?php

abstract class BadAbstract implements Operable {
    abstract public function __add(int $x): self;
}

?>
--EXPECTF--
Fatal error: Class BadAbstract implements Operable and must be declared readonly in %s on line %d
