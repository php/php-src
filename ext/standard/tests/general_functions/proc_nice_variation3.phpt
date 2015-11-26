--TEST--
Test function proc_nice() by substituting argument 1 with emptyUnsetUndefNull values.
--CREDITS--
Italian PHP TestFest 2009 Cesena 19-20-21 june
Fabio Fabbrucci (fabbrucci@grupporetina.com)
Michele Orselli (mo@ideato.it)
Simone Gentili (sensorario@gmail.com)
--SKIPIF--
<?php
if(!function_exists('proc_nice')) die("skip. proc_nice not available ");
?>
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
  'lowercase null' => null,
  );


foreach ( $variation_array as $var ) {
  var_dump(proc_nice( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with emptyUnsetUndefNull values ***
bool(true)
bool(true)

Warning: proc_nice() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

Warning: proc_nice() expects parameter 1 to be integer, string given in %s on line %d
bool(false)
bool(true)
bool(true)
