--TEST--
inflate_get_read_len()
--EXTENSIONS--
zlib
--FILE--
<?php

$uncompressed = "Hello world.";
$random_junk = str_repeat("qebsouesl", 128);

$compressed = zlib_encode($uncompressed, ZLIB_ENCODING_DEFLATE);
$compressed_len = strlen($compressed);
$compressed .= $random_junk;

$ctx = inflate_init(ZLIB_ENCODING_DEFLATE);
$buf = inflate_add($ctx, $compressed);
$detected_compressed_len = inflate_get_read_len($ctx);

echo 'Status: ' . inflate_get_status($ctx) . "\n";
echo 'Original compressed length: ' . $compressed_len . "\n";
echo 'Detected compressed length: ' . $detected_compressed_len . "\n";

echo ($compressed_len == $detected_compressed_len) ? 'The lengths are equal.' : 'The lengths are unequal.';
?>
--EXPECT--
Status: 1
Original compressed length: 20
Detected compressed length: 20
The lengths are equal.
