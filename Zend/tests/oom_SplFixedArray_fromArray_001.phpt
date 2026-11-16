--TEST--
OOM SplFixedArray::fromArray()
--INI--
memory_limit=10M
--FILE--
<?php

SplFixedArray::fromArray([10*1024*1024 => 0], true);

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 3
