--TEST--
OOM SplFixedArray::fromArray() 002
--INI--
memory_limit=11M
--FILE--
<?php

$array = array_fill(0, 128*1024, 0);
$pad = str_repeat('a', max(10*1024*1024 - memory_get_usage(true) - 512*1024, 1));
SplFixedArray::fromArray($array, false);

?>
--EXPECTF--
Fatal error: Allowed memory size of 11534336 bytes exhausted %S(tried to allocate %d bytes) in %s on line 5
