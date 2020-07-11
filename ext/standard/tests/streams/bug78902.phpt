--TEST--
Bug #78902: Memory leak when using stream_filter_append
--INI--
memory_limit=512k
--FILE--
<?php

/** create temporary file 2mb file */
$tmp_file_name = tempnam(sys_get_temp_dir(), 'test_');
$fp = fopen($tmp_file_name, 'w+');
$size = 1024 * 1024 * 2; // 2mb
$chunk = 1024;
while ($size > 0) {
    fputs($fp, str_pad('', min($chunk,$size)));
    $size -= $chunk;
}
fclose($fp);

$fp = fopen($tmp_file_name, 'r');
stream_filter_append($fp, "string.toupper");
while (!feof($fp)) {
    fread($fp, 1);
}
fclose($fp);
var_dump(true);
?>
--EXPECT--
bool(true)
