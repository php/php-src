--TEST--
Test function gzfile() by substituting argument 2 with object values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$filename = $filename = dirname(__FILE__)."/004.txt.gz";


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
  var_dump(gzfile( $filename, $var  ) );
}
?>
===DONE===
--EXPECTF--
Error: 2 - gzfile() expects parameter 2 to be integer, object given, %s(%d)
NULL
Error: 2 - gzfile() expects parameter 2 to be integer, object given, %s(%d)
NULL
===DONE===
