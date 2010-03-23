--TEST--
Test function gzfile() by substituting agument 1 with array values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$use_include_path = false;


$index_array = array(1, 2, 3);
$assoc_array = array(1 => 'one', 2 => 'two');

$variation = array(
  'empty array' => array(),
  'int indexed array' => $index_array,
  'associative array' => $assoc_array,
  'nested arrays' => array('foo', $index_array, $assoc_array),
  );


foreach ( $variation as $var ) {
  var_dump(gzfile( $var ,  $use_include_path ) );
}
?>
===DONE===
--EXPECTF--

Warning: gzfile() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: gzfile() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: gzfile() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: gzfile() expects parameter 1 to be string, array given in %s on line %d
NULL
===DONE===