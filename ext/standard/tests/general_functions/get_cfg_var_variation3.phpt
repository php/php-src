--TEST--
Test function get_cfg_var() by substituting argument 1 with emptyUnsetUndefNull values.
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
session.use_cookies=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php


echo "*** Test substituting argument 1 with emptyUnsetUndefNull values ***\n";



$unset_var = 10;
unset($unset_var);

$variation_array = array(
  'unset var' => @$unset_var,
  'undefined var' => @$undefined_var,
  'empty string DQ' => "",
  'empty string SQ' => '',
  'uppercase NULL' => NULL,
  'lowercase null' => null
  );


foreach ( $variation_array as $var ) {
  var_dump(get_cfg_var( $var  ) );
}
?>
--EXPECT--
*** Test substituting argument 1 with emptyUnsetUndefNull values ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
