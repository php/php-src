--TEST--
mb_strcut() doesn't reach hard limit - encodings without a length table
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
memory_hard_limit_percent=125
--FILE--
<?php
// UTF-7 has no length table, so mb_strcut() converts through a filter chain
// into a memory device sized for the input. UTF-7 re-encodes "_" as "+AF8-",
// so the device has to grow while the limit is already exhausted.
// 2 MB == memory_limit / 4
$string = str_repeat("_", 2 * 1024 * 1024);
mb_strcut($string, 0, strlen($string), "UTF-7");
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
