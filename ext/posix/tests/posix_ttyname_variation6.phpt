--TEST--
Test function posix_ttyname() by substituting argument 1 with object values.
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
  var_dump(posix_ttyname( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with object values ***
Error: 8 - Object of class classWithToString could not be converted to int, %s(%d)
bool(false)
Error: 8 - Object of class classWithoutToString could not be converted to int, %s(%d)
bool(false)
