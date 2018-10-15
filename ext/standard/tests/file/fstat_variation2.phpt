--TEST--
Test function fstat() by substituting argument 1 with boolean values.
--FILE--
<?php
$variation_array = array(
  'lowercase true' => true,
  'lowercase false' =>false,
  'uppercase TRUE' =>TRUE,
  'uppercase FALSE' =>FALSE,
  );


foreach ( $variation_array as $var ) {
  var_dump(fstat( $var  ) );
}
?>
===DONE===
--EXPECTF--
Warning: fstat() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)
===DONE===
