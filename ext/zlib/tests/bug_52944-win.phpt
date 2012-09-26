--TEST--
Bug #52944 (segfault with zlib filter and corrupted data)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
	die("skip windows only");
}
--INI--
allow_url_fopen=1
--FILE--
<?php
require dirname(__FILE__) . "/bug_52944_corrupted_data.inc";

$fp = fopen('data://text/plain;base64,' . $data, 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);
var_dump(fread($fp,1));
var_dump(fread($fp,1));
fclose($fp);
echo "Done.\n";
--EXPECT--
string(1) "%"
string(1) "C"
Done.
