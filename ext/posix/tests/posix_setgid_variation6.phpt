--TEST--
Test function posix_setgid() by substituting argument 1 with object values.
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
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
  var_dump(posix_setgid( $var  ) );
}
?>
===DONE===
--EXPECTF--
*** Test substituting argument 1 with object values ***
Error: 2 - posix_setgid() expects parameter 1 to be integer, object given, %s
bool(false)
Error: 2 - posix_setgid() expects parameter 1 to be integer, object given, %s
bool(false)
===DONE===
	
