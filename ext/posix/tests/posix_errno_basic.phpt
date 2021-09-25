--TEST--
Test function posix_errno() by calling it with its expected arguments
--CREDITS--
Morten Amundsen mor10am@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--EXTENSIONS--
posix
--FILE--
<?php

echo "*** Test by calling method or function with its expected arguments ***\n";

// test without any error
var_dump(posix_errno());

?>
--EXPECT--
*** Test by calling method or function with its expected arguments ***
int(0)
