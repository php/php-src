--TEST--
Test function pcntl_fork() by calling it with its expected arguments
--CREDITS--
Marco Fabbri mrfabbri@gmail.com
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
echo "*** Test by calling method or function with its expected arguments, first print the child PID and the father ***\n";

$pid = pcntl_fork();
if ($pid > 0) {
    pcntl_wait($status);
    var_dump($pid);
} else {
    var_dump($pid);
}
?>
--EXPECTF--
*** Test by calling method or function with its expected arguments, first print the child PID and the father ***
int(0)
int(%d)
