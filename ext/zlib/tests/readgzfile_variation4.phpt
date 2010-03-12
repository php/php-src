--TEST--
Test function readgzfile() by substituting agument 1 with float values.
--SKIPIF--
<?php
if (!extension_loaded(zlib)) die ('skip zlib extension not available in this build');
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
  var_dump(readgzfile( $var ,  $use_include_path ) );
}
?>
===DONE===
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
===DONE===
