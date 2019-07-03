--TEST--
Test function posix_errno() by calling it with its expected arguments
--CREDITS--
Morten Amundsen mor10am@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
--FILE--
<?php
echo "*** Test by calling function with pid error ***\n";

posix_kill((2 ** 22) + 1, SIGKILL);

var_dump(posix_errno());
?>
--EXPECT--
*** Test by calling function with pid error ***
int(3)
