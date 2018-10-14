--TEST--
Test function readgzfile() by substituting argument 2 with array values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$filename = dirname(__FILE__)."/004.txt.gz";


$index_array = array(1, 2, 3);
$assoc_array = array(1 => 'one', 2 => 'two');

$variation = array(
  'empty array' => array(),
  'int indexed array' => $index_array,
  'associative array' => $assoc_array,
  'nested arrays' => array('foo', $index_array, $assoc_array),
  );


foreach ( $variation as $var ) {
  var_dump(readgzfile( $filename, $var  ) );
}
?>
===DONE===
--EXPECTF--
Warning: readgzfile() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: readgzfile() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: readgzfile() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: readgzfile() expects parameter 2 to be integer, array given in %s on line %d
NULL
===DONE===
