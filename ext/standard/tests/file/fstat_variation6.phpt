--TEST--
Test function fstat() by substituting argument 1 with object values.
--FILE--
<?php




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
  var_dump(fstat( $var  ) );
}
?>
===DONE===
--EXPECTF--
Error: 2 - fstat() expects parameter 1 to be resource, object given, %s(%d)
bool(false)
Error: 2 - fstat() expects parameter 1 to be resource, object given, %s(%d)
bool(false)
===DONE===
