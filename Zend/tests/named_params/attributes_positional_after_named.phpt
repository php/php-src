--TEST--
Named params in attributes: Positional after named error
--FILE--
<?php

#[Attribute]
class MyAttribute { }

#[MyAttribute(a: 'A', 'B')]
class Test {}

?>
--EXPECTF--
Fatal error: Cannot use positional argument after named argument in %s on line %d
