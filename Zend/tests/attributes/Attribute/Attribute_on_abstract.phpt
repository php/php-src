--TEST--
#[Attribute] on an abstract class
--FILE--
<?php

#[Attribute]
abstract class Demo {}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot apply #[\Attribute] to abstract class Demo in %s on line %d
