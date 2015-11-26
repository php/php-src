--TEST--
Test function proc_nice() by substituting argument 1 with boolean values.
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


echo "*** Test substituting argument 1 with boolean values ***\n";



$variation_array = array(
  'lowercase true' => true,
  'lowercase false' =>false,
  'uppercase TRUE' =>TRUE,
  'uppercase FALSE' =>FALSE,
  );


foreach ( $variation_array as $var ) {
  var_dump(proc_nice( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with boolean values ***
bool(true)
bool(true)
bool(true)
bool(true)
