--TEST--
Test function gzfile() by substituting argument 1 with boolean values.
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
  var_dump(gzfile( $var ,  $use_include_path ) );
}
?>
===DONE===
--EXPECTF--
Warning: gzfile() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: gzfile() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: gzfile() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: gzfile() expects parameter 2 to be integer, array given in %s on line %d
NULL
===DONE===
