--TEST--
Closures as a signal handler
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
declare (ticks = 1);

pcntl_signal(SIGTERM, function ($signo) { echo "Signal handler called!\n"; });

echo "Start!\n";
posix_kill(posix_getpid(), SIGTERM);
$i = 0; // dummy
echo "Done!\n";

?>
--EXPECT--
Start!
Signal handler called!
Done!
