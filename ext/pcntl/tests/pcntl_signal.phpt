--TEST--
pcntl_signal()
--SKIPIF--
<?php if (!extension_loaded("pcntl")) print "skip"; ?>
<?php if (!extension_loaded("posix")) die("skip posix extension not available"); ?>
--FILE--
<?php
pcntl_signal(SIGTERM, function($signo){
	echo "signal dispatched\n";
});
posix_kill(posix_getpid(), SIGTERM);
pcntl_signal_dispatch();

var_dump(pcntl_signal());
var_dump(pcntl_signal(SIGALRM, SIG_IGN));
var_dump(pcntl_signal(-1, -1));
var_dump(pcntl_signal(-1, function(){}));
var_dump(pcntl_signal(SIGALRM, "not callable"));


/* test freeing queue in RSHUTDOWN */
posix_kill(posix_getpid(), SIGTERM);
echo "ok\n";
?>
--EXPECTF--
signal dispatched

Warning: pcntl_signal() expects at least 2 parameters, 0 given in %s
NULL
bool(true)

Warning: pcntl_signal(): Invalid signal %s
bool(false)

Warning: pcntl_signal(): Invalid signal %s
bool(false)

Warning: pcntl_signal(): not callable is not a callable function name error in %s
bool(false)
ok
