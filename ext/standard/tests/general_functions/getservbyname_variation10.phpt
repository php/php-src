--TEST--
Test function getservbyname() by substituting argument 2 with emptyUnsetUndefNull values.
--SKIPIF--
<?php
if(PHP_OS == 'Darwin') {
  die("skip.. Mac OS X is fine with NULLs in getservbyname");
}
?>
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
--EXPECTF--
*** Test substituting argument 2 with emptyUnsetUndefNull values ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
