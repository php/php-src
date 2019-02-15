--TEST--
Test function fstat() by substituting argument 1 with array values.
--FILE--
<?php
$index_array = array(1, 2, 3);
$assoc_array = array(1 => 'one', 2 => 'two');

$variation_array = array(
  'empty array' => array(),
  'int indexed array' => $index_array,
  'associative array' => $assoc_array,
  'nested arrays' => array('foo', $index_array, $assoc_array),
  );


foreach ( $variation_array as $var ) {
  var_dump(fstat( $var  ) );
}
?>
===DONE===
--EXPECTF--
Warning: fstat() expects parameter 1 to be resource, array given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, array given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, array given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, array given in %s on line %d
bool(false)
===DONE===
