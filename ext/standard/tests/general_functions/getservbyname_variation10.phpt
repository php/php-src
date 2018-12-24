--TEST--
Test function getservbyname() by substituting argument 2 with emptyUnsetUndefNull values.
--FILE--
<?php


echo "*** Test substituting argument 2 with emptyUnsetUndefNull values ***\n";

$service = "www";


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
  var_dump(getservbyname( $service, $var  ) );
}
?>
--EXPECT--
*** Test substituting argument 2 with emptyUnsetUndefNull values ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
