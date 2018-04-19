--TEST--
Test function posix_ttyname() by substituting argument 1 with boolean values.
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


echo "*** Test substituting argument 1 with boolean values ***\n";



$variation_array = array(
  'lowercase true' => true,
  'lowercase false' =>false,
  'uppercase TRUE' =>TRUE,
  'uppercase FALSE' =>FALSE,
  );


foreach ( $variation_array as $var ) {
  var_dump(posix_ttyname( $var  ) );
}
?>
--EXPECT--
*** Test substituting argument 1 with boolean values ***
bool(false)
bool(false)
bool(false)
bool(false)
