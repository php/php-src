--TEST--
Test function readgzfile() by substituting agument 1 with object values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$use_include_path = false;


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

$variation = array(
  'instance of classWithToString' => new classWithToString(),
  'instance of classWithoutToString' => new classWithoutToString(),
  );


foreach ( $variation as $var ) {
  var_dump(readgzfile( $var ,  $use_include_path ) );
}
?>
--EXPECTF--
bool(false)
Error: 2 - readgzfile() expects parameter 1 to be string, object given, %s(%d)
NULL