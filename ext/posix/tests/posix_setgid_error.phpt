--TEST--
Test function posix_setgid() by calling it more than or less than its expected arguments.
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php


echo "*** Test by calling method or function with incorrect numbers of arguments ***\n";

$gid = posix_getgid();
$extra_arg = '123';

var_dump(posix_setgid( $gid, $extra_arg ) );
var_dump(posix_setgid(  ) );

?>
===DONE===
--EXPECTF--
*** Test by calling method or function with incorrect numbers of arguments ***

Warning: posix_setgid() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

Warning: posix_setgid() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
===DONE===
