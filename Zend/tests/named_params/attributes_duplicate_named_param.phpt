--TEST--
Named params in attributes: Duplicate named parameter error
--FILE--
<?php

#[MyAttribute(a: 'A', a: 'A')]
class Test {}

?>
--EXPECTF--
Fatal error: Duplicate named parameter $a in %s on line %d
