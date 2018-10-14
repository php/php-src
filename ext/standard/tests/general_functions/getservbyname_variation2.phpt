--TEST--
Test function getservbyname() by substituting argument 1 with boolean values.
--FILE--
<?php


echo "*** Test substituting argument 1 with boolean values ***\n";

$protocol = "tcp";


$variation_array = array(
  'lowercase true' => true,
  'lowercase false' =>false,
  'uppercase TRUE' =>TRUE,
  'uppercase FALSE' =>FALSE,
  );


foreach ( $variation_array as $var ) {
  var_dump(getservbyname( $var ,  $protocol ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with boolean values ***
bool(false)
bool(false)
bool(false)
bool(false)
