--TEST--
#[Attribute] on an enum
--FILE--
<?php

#[Attribute]
enum Demo {}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot apply #[\Attribute] to enum Demo in %s on line %d
