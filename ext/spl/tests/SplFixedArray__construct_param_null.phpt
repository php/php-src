--TEST--
SplFixedArray::__construct() with null passed as parameter.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplFixedArray( NULL );

print_r( $array );

?>
--EXPECTF--
Deprecated: SplFixedArray::__construct(): Passing null to parameter #1 ($size) of type int is deprecated in %s on line %d
SplFixedArray Object
(
)
