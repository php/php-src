--TEST--
Test function readgzfile() by substituting argument 1 with boolean values.
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build');
?>
--FILE--
<?php


$use_include_path =


$variation = array(
  'lowercase true' => true,
  'lowercase false' =>false,
  'uppercase TRUE' =>TRUE,
  'uppercase FALSE' =>FALSE,
  );


foreach ( $variation as $var ) {
  var_dump(readgzfile( $var ,  $use_include_path ) );
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
