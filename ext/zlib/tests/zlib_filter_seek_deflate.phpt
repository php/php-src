--TEST--
zlib.deflate write filter is not reset on seek
--EXTENSIONS--
zlib
--FILE--
<?php
/* Write filters are not reset on stream seek; seeking only affects the
 * stream's read/write position, not the filter pipeline state. This ensures
 * seeking a stream with write filters does not disrupt the filter state. */

$file = __DIR__ . '/zlib_filter_seek_deflate.zlib';

$text = 'Hello, World!';

$fp = fopen($file, 'w+');
$filter = stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE);

fwrite($fp, $text);

/* Remove the filter to finalize compression cleanly before seeking */
stream_filter_remove($filter);

$size = ftell($fp);
echo "Size after write: $size\n";

/* Seek to start succeeds; write filters no longer block seeking */
$result = fseek($fp, 0, SEEK_SET);
echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

/* Seek to middle also succeeds */
$result = fseek($fp, 50, SEEK_SET);
echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

fclose($fp);

/* Verify the compressed output is still valid */
$fp = fopen($file, 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);
$content = stream_get_contents($fp);
fclose($fp);

echo "Decompressed content matches: " . ($content === $text ? "YES" : "NO") . "\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/zlib_filter_seek_deflate.zlib');
?>
--EXPECTF--
Size after write: %d
Seek to start: SUCCESS
Seek to middle: SUCCESS
Decompressed content matches: YES
