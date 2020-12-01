--TEST--
Bug #80384 large reads cause filters to internally buffer large amounts of memory
--FILE--
<?php
/* First, create a file to read */
$tmp_filename = __DIR__ . "/bug80384.tmp";
$fp = fopen($tmp_filename, 'w');
for ($i=0; $i<1024; $i++) {
    fwrite($fp, str_repeat('ABCDEFGH', 1024));
}
fclose($fp);

/* Stream the file through a filter */
$fp = fopen($tmp_filename, 'r');
$filter = stream_filter_append($fp, "string.rot13");

$mem_start = memory_get_usage();
fread($fp, 8 * 1024 * 1024);
$mem_final = memory_get_usage();
fclose($fp);
var_dump($mem_final - $mem_start < 32768);
?>
--CLEAN--
<?php
unlink(__DIR__ . "/bug80384.tmp");
?>
--EXPECT--
bool(true)
