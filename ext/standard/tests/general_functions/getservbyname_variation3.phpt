--TEST--
Test function getservbyname() by substituting argument 1 with emptyUnsetUndefNull values.
--FILE--
<?php


echo "*** Test substituting argument 1 with emptyUnsetUndefNull values ***\n";

$protocol = "tcp";


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
  var_dump(getservbyname( $var ,  $protocol ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with emptyUnsetUndefNull values ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
