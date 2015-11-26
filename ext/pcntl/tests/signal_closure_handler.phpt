--TEST--
Closures as a signal handler
--SKIPIF--
<?php
	if (!extension_loaded("pcntl")) print "skip"; 
	elseif (!function_exists("pcntl_signal")) print "skip pcntl_signal() not available";
	elseif (!function_exists("posix_kill")) print "skip posix_kill() not available";
	elseif (!function_exists("posix_getpid")) print "skip posix_getpid() not available";
?>
--FILE--
<?php
declare (ticks = 1);

pcntl_signal(SIGTERM, function ($signo) { echo "Signal handler called!\n"; });

echo "Start!\n";
posix_kill(posix_getpid(), SIGTERM);
$i = 0; // dummy
echo "Done!\n";

?>
--EXPECTF--
Start!
Signal handler called!
Done!
