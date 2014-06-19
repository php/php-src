--TEST--
Test function getservbyname() by substituting argument 2 with float values.
--FILE--
<?php


echo "*** Test substituting argument 2 with float values ***\n";

$service = "www";

$variation_array = array(
  'float 10.5' => 10.5,
  'float -10.5' => -10.5,
  'float 12.3456789000e10' => 12.3456789000e10,
  'float -12.3456789000e10' => -12.3456789000e10,
  'float .5' => .5,
  );


foreach ( $variation_array as $var ) {
  var_dump(getservbyname( $service, $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 2 with float values ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
