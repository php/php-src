--TEST--
Test function get_cfg_var() by substituting argument 1 with float values.
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
session.use_cookies=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php


echo "*** Test substituting argument 1 with float values ***\n";



$variation_array = array(
  'float 10.5' => 10.5,
  'float -10.5' => -10.5,
  'float 12.3456789000e10' => 12.3456789000e10,
  'float -12.3456789000e10' => -12.3456789000e10,
  'float .5' => .5,
  );


foreach ( $variation_array as $var ) {
  var_dump(get_cfg_var( $var  ) );
}
?>
--EXPECT--
*** Test substituting argument 1 with float values ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
