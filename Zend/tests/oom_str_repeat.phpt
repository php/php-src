--TEST--
OOM str_repeat()
--INI--
memory_limit=100M
--FILE--
<?php

var_dump(str_repeat('a', 300<<20));

?>
--EXPECTF--
Fatal error: Allowed memory size of 104857600 bytes exhausted %S(tried to allocate %d bytes) in %s on line 3
