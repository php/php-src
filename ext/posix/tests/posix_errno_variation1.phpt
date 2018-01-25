--TEST--
Test function posix_errno() by calling it with with permission error
--CREDITS--
Morten Amundsen mor10am@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
        if(posix_getuid()==0) print "skip - Cannot run test as root.";
?>
--FILE--
<?php

echo "*** Test by calling function with permission error ***\n";

posix_setuid(0);
var_dump(posix_errno());

?>
--EXPECTF--
*** Test by calling function with permission error ***
int(1)
