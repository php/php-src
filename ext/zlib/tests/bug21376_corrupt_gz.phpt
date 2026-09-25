--TEST--
Bug #21376: gzfile and readgzfile must detect corrupted gzip data
--EXTENSIONS--
zlib
--FILE--
<?php
/* Minimal invalid gzip: magic + garbage so zlib reports Z_DATA_ERROR */
$corrupt = __DIR__ . '/bug21376_corrupt.gz';
file_put_contents($corrupt, "\x1f\x8b\x08\x00\x00\x00\x00\x00\x00\x03" . "invalid compressed data");

$result = @gzfile($corrupt);
var_dump($result);

ob_start();
$result = @readgzfile($corrupt);
ob_end_clean();
var_dump($result);

@unlink($corrupt);
?>
--EXPECT--
array(0) {
}
int(-1)
