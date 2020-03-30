--TEST--
Bug #78076	Opcache greatly increases memory usage.
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache="{TMP}"
opcache.file_cache_only=1
--SKIPIF--
--FILE--
<?php
$start =  memory_get_usage();
for ($x = 1; $x < 10000; $x += 1) {
    include("leaktest.inc");
}
$end = memory_get_usage();
var_dump($end - $start < 1000);
?>
--EXPECT--
bool(true)

