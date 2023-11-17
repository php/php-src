--TEST--
Bug #52944 (segfault with zlib filter and corrupted data)
--EXTENSIONS--
zlib
--INI--
allow_url_fopen=1
--FILE--
<?php
/* NOTE this test can fail on asm builds of zlib 1.2.5 or
   1.2.7 on at least Windows and Darwin. Using unoptimized
   zlib build fixes the issue. */

require __DIR__ . "/bug_52944_corrupted_data.inc";

$fp = fopen('data://text/plain;base64,' . $data, 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);
try {
    var_dump(fread($fp,1));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(fread($fp,1));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}
fclose($fp);
echo "Done.\n";
?>
--EXPECTF--
zlib: data error
string(0) ""
Done.
