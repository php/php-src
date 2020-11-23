--TEST--
Test function proc_nice() by substituting argument 1 with int values.
--CREDITS--
Italian PHP TestFest 2009 Cesena 19-20-21 june
Fabio Fabbrucci (fabbrucci@grupporetina.com)
Michele Orselli (mo@ideato.it)
Simone Gentili (sensorario@gmail.com)
--SKIPIF--
<?php
    if(!function_exists('proc_nice')) die("skip. proc_nice not available ");
    if(!function_exists('posix_geteuid')) die("skip. posix_geteuid not available ");
    if(posix_geteuid() == 0) print "skip - Cannot run test as root.";
?>
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
  var_dump(proc_nice( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with int values ***
bool(true)
bool(true)
bool(true)

Warning: proc_nice(): Only a super user may attempt to increase the priority of a process in %s on line %d
bool(false)
