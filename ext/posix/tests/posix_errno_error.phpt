--TEST--
Test function posix_errno() by calling it with its expected arguments
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
?>
--CREDITS--
Morten Amundsen mor10am@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php

echo "*** Test by calling method or function with more than expected arguments ***\n";

// test without any error
var_dump(posix_errno('bar'));

?>
--EXPECTF--
*** Test by calling method or function with more than expected arguments ***

Warning: Wrong parameter count for posix_errno() in %s.php on line %d
NULL
