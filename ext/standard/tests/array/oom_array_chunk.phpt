--TEST--
array_chunk() doesn't reach hard limit
--INI--
memory_limit=8M
--FILE--
<?php
$input = array_fill(0, 250000, 0);
array_chunk($input, 1);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
