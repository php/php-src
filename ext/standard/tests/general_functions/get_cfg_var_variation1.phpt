--TEST--
Test function get_cfg_var() by substituting argument 1 with array values.
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
session.use_cookies=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php


echo "*** Test substituting argument 1 with array values ***\n";



$index_array = array(1, 2, 3);
$assoc_array = array(1 => 'one', 2 => 'two');

$variation_array = array(
  'empty array' => array(),
  'int indexed array' => $index_array,
  'associative array' => $assoc_array,
  'nested arrays' => array('foo', $index_array, $assoc_array),
  );


foreach ( $variation_array as $var ) {
  var_dump(get_cfg_var( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with array values ***

Warning: get_cfg_var() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: get_cfg_var() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: get_cfg_var() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: get_cfg_var() expects parameter 1 to be string, array given in %s on line %d
NULL
