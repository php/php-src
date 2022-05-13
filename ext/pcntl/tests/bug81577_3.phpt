--TEST--
Bug #81577: (Exceptions in interrupt handlers: cleanup_live_vars)
--EXTENSIONS--
pcntl
posix
--XFAIL--
leaks are not fixed yet
--FILE--
<?php
pcntl_async_signals(true);
pcntl_signal(SIGTERM, function ($signo) { throw new Exception("Signal"); });
try {
	array_merge([1], [2]) + posix_kill(posix_getpid(), SIGTERM);
} catch (Throwable $ex) {
	echo get_class($ex) , " : " , $ex->getMessage() , "\n";
}
?>
--EXPECT--
Exception : Signal
