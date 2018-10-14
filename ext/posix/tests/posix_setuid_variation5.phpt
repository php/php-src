--TEST--
Test function posix_setuid() by substituting argument 1 with int values.
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
        if(posix_geteuid() == 0) print "skip - Cannot run test as root.";
?>
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php


echo "*** Test substituting argument 1 with int values ***\n";



$variation_array = array (
    'int 0' => 0,
    'int 1' => 1,
    'int -12345' => -12345,
    );


foreach ( $variation_array as $var ) {
  var_dump(posix_setuid( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with int values ***
bool(false)
bool(false)
bool(false)
