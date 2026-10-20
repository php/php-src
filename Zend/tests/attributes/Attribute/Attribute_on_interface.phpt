--TEST--
#[Attribute] on an interface
--FILE--
<?php

#[Attribute]
interface Demo {}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot apply #[\Attribute] to interface Demo in %s on line %d
