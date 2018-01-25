--TEST--
Test function posix_errno() by calling it with its expected arguments
--CREDITS--
Morten Amundsen mor10am@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--SKIPIF--
<?php
        if(!extension_loaded("posix")) print "skip - POSIX extension not loaded";
        if(!extension_loaded("pcntl")) print "skip - PCNTL extension required";
?>
--FILE--
<?php

echo "*** Test by calling function with pid error ***\n";

$pid = 10000;

do {
  $pid += 1;
  $result = shell_exec("ps -p " . $pid);
} while (strstr($pid, $result));

posix_kill($pid, SIGKILL);
var_dump(posix_errno());

?>
--EXPECTF--
*** Test by calling function with pid error ***
int(3)
