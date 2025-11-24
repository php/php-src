--TEST--
zlib.inflate filter with seek to start
--EXTENSIONS--
zlib
--FILE--
<?php
$file = __DIR__ . '/zlib_filter_seek_inflate.zlib';

$text = 'I am the very model of a modern major general, I\'ve information vegetable, animal, and mineral.';

$fp = fopen($file, 'w');
stream_filter_append($fp, 'zlib.deflate', STREAM_FILTER_WRITE);
fwrite($fp, $text);
fclose($fp);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);

$partial = fread($fp, 20);
echo "First read (20 bytes): " . $partial . "\n";

$result = fseek($fp, 0, SEEK_SET);
echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";
echo "Position after seek: " . ftell($fp) . "\n";

$full = stream_get_contents($fp);
echo "Content after seek matches: " . ($full === $text ? "YES" : "NO") . "\n";

$result = fseek($fp, 50, SEEK_SET);
echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

fclose($fp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/zlib_filter_seek_inflate.zlib');
?>
--EXPECTF--
First read (20 bytes): I am the very model 
Seek to start: SUCCESS
Position after seek: 0
Content after seek matches: YES

Warning: fseek(): Stream filter zlib.inflate is not seekable in %s on line %d
Seek to middle: FAILURE
