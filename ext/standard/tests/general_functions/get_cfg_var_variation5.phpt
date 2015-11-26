--TEST--
Test function get_cfg_var() by substituting argument 1 with int values.
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
session.use_cookies=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php


echo "*** Test substituting argument 1 with int values ***\n";



$variation_array = array (
    'int 0' => 0,
    'int 1' => 1,
    'int 12345' => 12345,
    'int -12345' => -2345,
    );


foreach ( $variation_array as $var ) {
  var_dump(get_cfg_var( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with int values ***
bool(false)
bool(false)
bool(false)
bool(false)
