--TEST--
Test function gzfile() by substituting argument 1 with int values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$use_include_path = false;


$variation = array (
    'int 0' => 0,
    'int 1' => 1,
    'int 12345' => 12345,
    'int -12345' => -2345,
    );


foreach ( $variation as $var ) {
  var_dump(gzfile( $var ,  $use_include_path ) );
}
?>
===DONE===
--EXPECTF--
Warning: gzfile(): failed to open stream (0): No such file or directory in %s on line %d
bool(false)

Warning: gzfile(): failed to open stream (1): No such file or directory in %s on line %d
bool(false)

Warning: gzfile(): failed to open stream (12345): No such file or directory in %s on line %d
bool(false)

Warning: gzfile(): failed to open stream (-2345): No such file or directory in %s on line %d
bool(false)
===DONE===
