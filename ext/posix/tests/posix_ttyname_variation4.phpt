--TEST--
Test function posix_ttyname() by substituting argument 1 with float values.
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


echo "*** Test substituting argument 1 with float values ***\n";



$variation_array = array(
  'float 10.5' => 10.5,
  'float -10.5' => -10.5,
  'float 12.3456789000e10' => 12.3456789000e10,
  'float -12.3456789000e10' => -12.3456789000e10,
  'float .5' => .5,
  );


foreach ( $variation_array as $var ) {
  var_dump(posix_ttyname( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with float values ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
