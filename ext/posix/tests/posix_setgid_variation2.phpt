--TEST--
Test function posix_setgid() by substituting argument 1 with boolean values.
--EXTENSIONS--
posix
--SKIPIF--
<?php
        if(posix_geteuid() == 0) print "skip - Cannot run test as root.";
?>
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
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
  var_dump(posix_setgid( $var  ) );
}
?>
--EXPECT--
*** Test substituting argument 1 with boolean values ***
bool(false)
bool(false)
bool(false)
bool(false)
