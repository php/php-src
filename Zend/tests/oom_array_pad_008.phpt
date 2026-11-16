--TEST--
OOM array_pad right mixed, non-refcounted pad value
--INI--
memory_limit=10M
--FILE--
<?php

var_dump(array_pad(['foo' => 'bar'], 1024*1024, 0x4141414141));

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 3
