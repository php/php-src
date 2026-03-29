--TEST--
Enum explicitly implementing Invokable without __invoke() causes fatal error
--FILE--
<?php

enum Color implements Invokable {
    case Red;
}

?>
--EXPECTF--
Fatal error: Enum Color must have an __invoke() method to implement Invokable in %s on line %d
