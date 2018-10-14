--TEST--
Test function posix_setgid() by substituting argument 1 with int values.
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
    'long 0' => 0,
    'long 1' => 1,
    'int -12345' => -2345,
    );


foreach ( $variation_array as $var ) {
  var_dump(posix_setgid( $var  ) );
}
?>
===DONE===
--EXPECT--
*** Test substituting argument 1 with int values ***
bool(false)
bool(false)
bool(false)
===DONE===
	
