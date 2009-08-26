--TEST--
Test function getservbyname() by substituting argument 2 with boolean values.
--FILE--
<?php


echo "*** Test substituting argument 2 with boolean values ***\n";

$service = "www";

$variation_array = array(
  'lowercase true' => true,
  'lowercase false' =>false,
  'uppercase TRUE' =>TRUE,
  'uppercase FALSE' =>FALSE,
  );


foreach ( $variation_array as $var ) {
  var_dump(getservbyname( $service, $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 2 with boolean values ***
bool(false)
bool(false)
bool(false)
bool(false)
