--TEST--
Test function fstat() by substituting argument 1 with emptyUnsetUndefNull values.
--FILE--
<?php
$unset_var = 10;
unset($unset_var);

$variation_array = array(
  'unset var' => @$unset_var,
  'undefined var' => @$undefined_var,
  'empty string DQ' => "",
  'empty string SQ' => '',
  'uppercase NULL' => NULL,
  'lowercase null' => null,
  );


foreach ( $variation_array as $var ) {
  var_dump(fstat( $var  ) );
}
?>
===DONE===
--EXPECTF--

Warning: fstat() expects parameter 1 to be resource, null given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, null given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, string given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, string given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, null given in %s on line %d
bool(false)

Warning: fstat() expects parameter 1 to be resource, null given in %s on line %d
bool(false)
===DONE===