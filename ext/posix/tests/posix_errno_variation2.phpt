--TEST--
Test function posix_errno() by calling it with its expected arguments
--CREDITS--
Morten Amundsen mor10am@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--EXTENSIONS--
posix
pcntl
--FILE--
<?php
echo "*** Test by calling function with pid error ***\n";

// Don't rely on PCNTL extension being around
$SIGKILL = 9;

posix_kill((2 ** 22) + 1, $SIGKILL);

var_dump(posix_errno());
?>
--EXPECT--
*** Test by calling function with pid error ***
int(3)
