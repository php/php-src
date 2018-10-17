--TEST--
Bug #52944 (segfault with zlib filter and corrupted data)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
/* NOTE this test can fail on asm builds of zlib 1.2.5 or
   1.2.7 on at least Windows and Darwin. Using unoptimized
   zlib build fixes the issue. */

require dirname(__FILE__) . "/bug_52944_corrupted_data.inc";

$fp = fopen('data://text/plain;base64,' . $data, 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);
var_dump(fread($fp,1));
var_dump(fread($fp,1));
fclose($fp);
echo "Done.\n";
--EXPECT--
string(0) ""
string(0) ""
Done.
