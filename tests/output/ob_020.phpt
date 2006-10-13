--TEST--
output buffering - memory exhaustion (segv, memory_limit>200k)
--SKIPIF--
<?php
function_exists("memory_get_usage") or die("need --enable-memory-limit\n");
?>
--INI--
output_buffering=1
memory_limit=500k
--FILE--
<?php
while(true) echo "foo";
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted at %s/output.c:%d (tried to allocate %d bytes) in Unknown on line 0
