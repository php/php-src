--TEST--
OOM json_decode() empty objects
--INI--
memory_limit=10M
--FILE--
<?php

$elems = '{}'.str_repeat(',{}', 1024*1024);
$json = '[' . $elems . ']';
var_dump(json_decode($json));

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 5
