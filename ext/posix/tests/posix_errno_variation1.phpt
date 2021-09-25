--TEST--
Test function posix_errno() by calling it with with permission error
--CREDITS--
Morten Amundsen mor10am@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--EXTENSIONS--
posix
--SKIPIF--
<?php
        if(posix_getuid()==0) print "skip - Cannot run test as root.";
?>
--FILE--
<?php

echo "*** Test by calling function with permission error ***\n";

posix_setuid(0);
var_dump(posix_errno());

?>
--EXPECT--
*** Test by calling function with permission error ***
int(1)
