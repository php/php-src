--TEST--
void memory_reset_peak_usage();
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}
?>
--INI--
memory_limit=-1
--FILE--
<?php
$array0 = range(1, 1024 * 1024);
$m0 = memory_get_peak_usage();
$array1 = range(1, 1024 * 1024);
var_dump(($m1 = memory_get_peak_usage()) > $m0);
unset($array0, $array1);
memory_reset_peak_usage();
var_dump(memory_get_peak_usage() < $m1);
?>
--EXPECT--
bool(true)
bool(true)
