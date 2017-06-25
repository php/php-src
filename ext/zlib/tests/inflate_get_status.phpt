--TEST--
inflate_get_status()
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php

$uncompressed = "Hello world.";
$random_junk = str_repeat("qebsouesl", 128);

$compressed = zlib_encode($uncompressed, ZLIB_ENCODING_DEFLATE);
$compressed_len = strlen($compressed);
$compressed .= $random_junk;

$ctx = inflate_init(ZLIB_ENCODING_DEFLATE);
$status = inflate_get_status($ctx);
$buf = '';

for ($i = 0; $status == ZLIB_OK; ++$i)
{
    $buf .= inflate_add($ctx, substr($compressed, $i, 1));
    echo '$i = ' . $i . ', ';
    $status = inflate_get_status($ctx);
    echo 'Status: ' . $status;
    echo "\n";
}

echo '$buf = ' . $buf;
echo "\n\n";

echo "Adding more data should reset the stream and result in a Z_OK (ZLIB_OK) status.\n";
inflate_add($ctx, substr($compressed, 0, 12));
echo 'Status: ' . inflate_get_status($ctx);

?>
--EXPECT--
$i = 0, Status: 0
$i = 1, Status: 0
$i = 2, Status: 0
$i = 3, Status: 0
$i = 4, Status: 0
$i = 5, Status: 0
$i = 6, Status: 0
$i = 7, Status: 0
$i = 8, Status: 0
$i = 9, Status: 0
$i = 10, Status: 0
$i = 11, Status: 0
$i = 12, Status: 0
$i = 13, Status: 0
$i = 14, Status: 0
$i = 15, Status: 0
$i = 16, Status: 0
$i = 17, Status: 0
$i = 18, Status: 0
$i = 19, Status: 1
$buf = Hello world.

Adding more data should reset the stream and result in a Z_OK (ZLIB_OK) status.
Status: 0
