--TEST--
Test function proc_nice() by substituting argument 1 with object values.
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


echo "*** Test substituting argument 1 with object values ***\n";



function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
        if (error_reporting() != 0) {
                // report non-silenced errors
                echo "Error: $err_no - $err_msg, $filename($linenum)\n";
        }
}
set_error_handler('test_error_handler');



class classWithToString
{
        public function __toString() {
                return "Class A object";
        }
}

class classWithoutToString
{
}

$variation_array = array(
  'instance of classWithToString' => new classWithToString(),
  'instance of classWithoutToString' => new classWithoutToString(),
  );


foreach ( $variation_array as $var ) {
  var_dump(proc_nice( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with object values ***
Error: 2 - proc_nice() expects parameter 1 to be long, object given, %s(%d)
bool(false)
Error: 2 - proc_nice() expects parameter 1 to be long, object given, %s(%d)
bool(false)
