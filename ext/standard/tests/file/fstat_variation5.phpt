--TEST--
Test function fstat() by substituting agument 1 with int values.
--FILE--
<?php
$variation_array = array (
    'int 0' => 0,
    'int 1' => 1,
    'int 12345' => 12345,
    'int -12345' => -2345,
    );


foreach ( $variation_array as $var ) {
  var_dump(fstat( $var  ) );
}
?>
===DONE===
--EXPECTF--

Warning: fstat(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: fstat(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: fstat(): supplied argument is not a valid stream resource in %s on line %d
bool(false)

Warning: fstat(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
===DONE===