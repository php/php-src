--TEST--
pcnt_signal_dispatch()
--EXTENSIONS--
pcntl
posix
--FILE--
<?php

pcntl_signal(SIGTERM, function ($signo) { echo "Signal handler called!\n"; });

echo "Start!\n";
posix_kill(posix_getpid(), SIGTERM);
$i = 0; // dummy
pcntl_signal_dispatch();
echo "Done!\n";

?>
--EXPECT--
Start!
Signal handler called!
Done!
