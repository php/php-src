--TEST--
Test function getservbyname() by substituting argument 2 with int values.
--FILE--
<?php


echo "*** Test substituting argument 2 with int values ***\n";

$service = "www";


$variation_array = array (
    'int 0' => 0,
    'int 1' => 1,
    'int 12345' => 12345,
    'int -12345' => -2345,
    );


foreach ( $variation_array as $var ) {
  var_dump(getservbyname( $service, $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 2 with int values ***
bool(false)
bool(false)
bool(false)
bool(false)
