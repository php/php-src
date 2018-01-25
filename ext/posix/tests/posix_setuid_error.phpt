--TEST--
Test function posix_setuid() by calling it more than or less than its expected arguments
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php


echo "*** Test by calling method or function with incorrect numbers of arguments ***\n";

$uid = '123';


$extra_arg = '12312';

var_dump(posix_setuid( $uid, $extra_arg ) );

var_dump(posix_setuid(  ) );


?>
--EXPECTF--
*** Test by calling method or function with incorrect numbers of arguments ***

Warning: posix_setuid() expects exactly 1 parameter, 2 given in %s on line 11
bool(false)

Warning: posix_setuid() expects exactly 1 parameter, 0 given in %s on line 13
bool(false)
