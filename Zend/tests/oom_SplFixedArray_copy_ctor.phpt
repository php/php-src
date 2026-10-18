--TEST--
OOM SplFixedArray copy ctor
--INI--
memory_limit=10M
--FILE--
<?php

$a = new SplFixedArray(64*1024);

$pad = str_repeat('a', max(10*1024*1024 - memory_get_usage(true) - 1*1024*1024, 1));

clone $a;

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 7
