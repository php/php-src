--TEST--
Test function posix_setgid() by substituting argument 1 with emptyUnsetUndefNull values.
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
        if(posix_geteuid() == 0) print "skip - Cannot run test as root.";
?>
--FILE--
<?php


echo "*** Test substituting argument 1 with emptyUnsetUndefNull values ***\n";



$unset_var = 10;
unset($unset_var);

$variation_array = array(
  'unset var' => @$unset_var,
  'undefined var' => @$undefined_var,
  'empty string DQ' => "",
  'empty string SQ' => '',
  'uppercase NULL' => NULL,
  'lowercase null' => null,
  );


foreach ( $variation_array as $var ) {
  var_dump(posix_setgid( $var  ) );
}
?>
--EXPECTF--
*** Test substituting argument 1 with emptyUnsetUndefNull values ***
bool(false)
bool(false)

Warning: posix_setgid() expects parameter 1 to be integer, string given in %s on line %d
bool(false)

Warning: posix_setgid() expects parameter 1 to be integer, string given in %s on line %d
bool(false)
bool(false)
bool(false)
