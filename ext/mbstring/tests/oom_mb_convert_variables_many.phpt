--TEST--
mb_convert_variables() doesn't reach hard limit - many strings
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
$values = array_fill(0, 200000, "abc");
mb_convert_variables("UTF-8", "UTF-8", $values);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
