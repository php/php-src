--TEST--
SplFixedArray::__construct() with null passed as parameter.
--CREDITS--
PHPNW Test Fest 2009 - Jordan Hatch
--FILE--
<?php

$array = new SplFixedArray( NULL );

print_r( $array );

?>
--EXPECT--
SplFixedArray Object
(
)
