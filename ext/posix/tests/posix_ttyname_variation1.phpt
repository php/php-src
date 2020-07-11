--TEST--
Test function posix_ttyname() by substituting argument 1 with array values.
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded('posix')) {
    die('SKIP The posix extension is not loaded.');
}
?>
--FILE--
<?php


echo "*** Test substituting argument 1 with array values ***\n";



$index_array = array(1, 2, 3);
$assoc_array = array(1 => 'one', 2 => 'two');

$variation_array = array(
  'empty array' => array(),
  'int indexed array' => $index_array,
  'associative array' => $assoc_array,
  'nested arrays' => array('foo', $index_array, $assoc_array),
  );


foreach ( $variation_array as $var ) {
  var_dump(posix_ttyname( $var  ) );
}
?>
--EXPECT--
*** Test substituting argument 1 with array values ***
bool(false)
bool(false)
bool(false)
bool(false)
