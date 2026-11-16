--TEST--
filter_var_array() doesn't reach hard limit
--EXTENSIONS--
filter
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die("skip ASan-instrumented realloc() is slow");
?>
--FILE--
<?php
$value = str_repeat("\xFF", 2 * 1024 * 1024);
filter_var_array(
    ["k" => $value],
    ["k" => ["filter" => FILTER_SANITIZE_SPECIAL_CHARS, "flags" => FILTER_FLAG_ENCODE_HIGH]]
);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
