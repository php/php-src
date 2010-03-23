--TEST--
Test function proc_nice() by substituting argument 1 with string values.
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


echo "*** Test substituting argument 1 with string values ***\n";



$heredoc = <<<EOT
hello world
EOT;

$variation_array = array(
  'string DQ' => "string",
  'string SQ' => 'string',
  'mixed case string' => "sTrInG",
  'heredoc' => $heredoc,
  );


foreach ( $variation_array as $var ) {
  var_dump(proc_nice( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with string values ***

Warning: proc_nice() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Warning: proc_nice() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Warning: proc_nice() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Warning: proc_nice() expects parameter 1 to be long, string given in %s on line %d
bool(false)
