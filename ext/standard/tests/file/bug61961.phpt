--TEST--
Bug #61961 (file_get_content leaks when access empty file with max length)
--FILE--
<?php
$tmp_empty_file = __FILE__ . ".tmp";
file_put_contents($tmp_empty_file, "");

var_dump(file_get_contents($tmp_empty_file, NULL, NULL, NULL, 10));
unlink($tmp_empty_file);
?>
==DONE==
--EXPECT--
string(0) ""
==DONE==