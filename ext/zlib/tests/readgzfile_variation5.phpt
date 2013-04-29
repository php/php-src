--TEST--
Test function readgzfile() by substituting agument 1 with int values.
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
  var_dump(readgzfile( $var ,  $use_include_path ) );
}
?>
===DONE===
--EXPECTF--
Warning: readgzfile(0): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(1): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(12345): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: readgzfile(-2345): failed to open stream: No such file or directory in %s on line %d
bool(false)
===DONE===
