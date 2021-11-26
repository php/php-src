--TEST--
Bug #81577: (Exceptions in interrupt handlers: ADD_ARRAY_ELEMENT)
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
pcntl_async_signals(true);
pcntl_signal(SIGTERM, function ($signo) {});
try {
	$a = [1, posix_kill(posix_getpid(), SIGTERM), 2];
} catch (Throwable $ex) {
	echo get_class($ex) , " : " , $ex->getMessage() , "\n";
}
var_dump($a);
?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  bool(true)
  [2]=>
  int(2)
}
