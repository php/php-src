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

pcntl_signal(SIGUSR1, function($signo, $siginfo){
    printf("got signal from %s\n", $siginfo['pid'] ?? 'nobody');
});
posix_kill(posix_getpid(), SIGUSR1);
pcntl_signal_dispatch();

var_dump(pcntl_signal(SIGALRM, SIG_IGN));

try {
    pcntl_signal(-1, -1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    pcntl_signal(-1, function(){});
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    pcntl_signal(SIGALRM, "not callable");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

/* test freeing queue in RSHUTDOWN */
posix_kill(posix_getpid(), SIGTERM);
echo "ok\n";
?>
--EXPECTF--
signal dispatched
got signal from %r\d+|nobody%r
bool(true)
pcntl_signal(): Argument #1 ($signal) must be greater than or equal to 1
pcntl_signal(): Argument #1 ($signal) must be greater than or equal to 1
pcntl_signal(): Argument #2 ($handler) must be of type callable|int, string given
ok
