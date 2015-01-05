--TEST--
Req #44164 (Handle "Content-Length" HTTP header when zlib.output_compression active)
--SKIPIF--
<?php
if (!function_exists('gzdeflate'))
    die("skip zlib extension required");
?>
--INI--
zlib.output_compression=On
--ENV--
HTTP_ACCEPT_ENCODING=gzip
--FILE--
<?php
header("Content-length: 200");
echo str_repeat("a", 200);
--EXPECT--
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
