--TEST--
Test function gzfile() by substituting argument 1 with emptyUnsetUndefNull values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$use_include_path = false;


$unset_var = 10;
unset($unset_var);

$variation = array(
  'unset var' => @$unset_var,
  'undefined var' => @$undefined_var,
  'empty string DQ' => "",
  'empty string SQ' => '',
  'uppercase NULL' => NULL,
  'lowercase null' => null,
  );


foreach ( $variation as $var ) {
  var_dump(gzfile( $var ,  $use_include_path ) );
}
?>
===DONE===
--EXPECTF--
Warning: gzfile(): Filename cannot be empty in %s on line %d
bool(false)

Warning: gzfile(): Filename cannot be empty in %s on line %d
bool(false)

Warning: gzfile(): Filename cannot be empty in %s on line %d
bool(false)

Warning: gzfile(): Filename cannot be empty in %s on line %d
bool(false)

Warning: gzfile(): Filename cannot be empty in %s on line %d
bool(false)

Warning: gzfile(): Filename cannot be empty in %s on line %d
bool(false)
===DONE===
