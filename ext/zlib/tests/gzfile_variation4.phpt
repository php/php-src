--TEST--
Test function gzfile() by substituting argument 1 with float values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$use_include_path = false;


$variation = array(
  'float 10.5' => 10.5,
  'float -10.5' => -10.5,
  'float 12.3456789000e10' => 12.3456789000e10,
  'float -12.3456789000e10' => -12.3456789000e10,
  'float .5' => .5,
  );


foreach ( $variation as $var ) {
  var_dump(gzfile( $var ,  $use_include_path ) );
}
?>
--EXPECTF--
Warning: gzfile(10.5): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: gzfile(-10.5): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: gzfile(123456789000): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: gzfile(-123456789000): Failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: gzfile(0.5): Failed to open stream: No such file or directory in %s on line %d
bool(false)
