--TEST--
#[Attribute] on a trait
--FILE--
<?php

#[Attribute]
trait Demo {}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot apply #[\Attribute] to trait Demo in %s on line %d
