--TEST--
Bug #81577: (Exceptions in interrupt handlers)
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
class C {
	public static $cond = 1;
	public static $a;
}

C::$a = [ C::$cond ]; // make countable zval

pcntl_async_signals(true);
pcntl_signal(SIGTERM, function ($signo) { throw new Exception("Signal"); });
for ($i = 0; $i < 5; $i++) {
	try {
		C::$a + C::$a;
		posix_kill(posix_getpid(), SIGTERM) + C::$cond;
	} catch (Throwable $ex) {
		echo get_class($ex) , " : " , $ex->getMessage() , "\n";
	}
}
?>
--EXPECT--
Exception : Signal
Exception : Signal
Exception : Signal
Exception : Signal
Exception : Signal
