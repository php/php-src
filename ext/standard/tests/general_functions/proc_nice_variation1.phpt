--TEST--
Test function proc_nice() by substituting argument 1 with array values.
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
  var_dump(proc_nice( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with array values ***

Warning: proc_nice() expects parameter 1 to be int, array given in %s on line %d
bool(false)

Warning: proc_nice() expects parameter 1 to be int, array given in %s on line %d
bool(false)

Warning: proc_nice() expects parameter 1 to be int, array given in %s on line %d
bool(false)

Warning: proc_nice() expects parameter 1 to be int, array given in %s on line %d
bool(false)
