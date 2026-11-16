--TEST--
OOM array_pad left packed
--INI--
memory_limit=10M
--FILE--
<?php

var_dump(array_pad([1,2,3], -1*1024*1024, (string)time()));

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 3
