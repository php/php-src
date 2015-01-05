--TEST--
pcnt_signal_dispatch()
--SKIPIF--
<?php
	if (!extension_loaded("pcntl")) print "skip"; 
	elseif (!function_exists("pcntl_signal")) print "skip pcntl_signal() not available";
	elseif (!function_exists("pcntl_signal_dispatch")) print "skip pcntl_signal_dispatch() not available";
	elseif (!function_exists("posix_kill")) print "skip posix_kill() not available";
	elseif (!function_exists("posix_getpid")) print "skip posix_getpid() not available";
?>
--FILE--
<?php

pcntl_signal(SIGTERM, function ($signo) { echo "Signal handler called!\n"; });

echo "Start!\n";
posix_kill(posix_getpid(), SIGTERM);
$i = 0; // dummy
pcntl_signal_dispatch();
echo "Done!\n";

?>
--EXPECTF--
Start!
Signal handler called!
Done!
