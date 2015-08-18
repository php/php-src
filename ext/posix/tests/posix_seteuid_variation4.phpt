--TEST--
Test function posix_seteuid() by substituting argument 1 with float values.
--SKIPIF--
<?php 
        PHP_INT_SIZE == 4 or die("skip - 32-bit only");
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
        if(posix_geteuid() == 0) print "skip - Cannot run test as root.";
?>
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php


echo "*** Test substituting argument 1 with float values ***\n";

$myUid = posix_getuid();

$myUid = $myUid - 1.1;

$variation_array = array(
  'float '.$myUid => $myUid,
  'float -'.$myUid => -$myUid,
  'float 12.3456789000e10' => 12.3456789000e10,
  'float -12.3456789000e10' => -12.3456789000e10,
  'float .5' => .5,
  );


foreach ( $variation_array as $var ) {
  var_dump(posix_seteuid( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with float values ***
bool(false)
bool(false)

Warning: posix_seteuid() expects parameter 1 to be integer, float given in %s on line %d
bool(false)

Warning: posix_seteuid() expects parameter 1 to be integer, float given in %s on line %d
bool(false)
bool(false)
