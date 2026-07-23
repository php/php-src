--TEST--
iconv stream filter with seek to start
--EXTENSIONS--
iconv
--FILE--
<?php
$file = __DIR__ . '/iconv_stream_filter_seek.txt';

$text = 'Hello, this is a test for iconv stream filter seeking functionality.';

$fp = fopen($file, 'w');
stream_filter_append($fp, 'convert.iconv.ISO-2022-JP/UTF-8');
fwrite($fp, $text);
fclose($fp);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'convert.iconv.UTF-8/ISO-2022-JP');

$partial = fread($fp, 20);
echo "First read (20 bytes): " . $partial . "\n";

$result = fseek($fp, 0, SEEK_SET);
echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

$full = fread($fp, strlen($text));
echo "Content after seek matches: " . ($full === $text ? "YES" : "NO") . "\n";

$result = fseek($fp, 50, SEEK_SET);
echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

fclose($fp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/iconv_stream_filter_seek.txt');
?>
--EXPECTF--
First read (20 bytes): Hello, this is a tes
Seek to start: SUCCESS
Content after seek matches: YES

Warning: fseek(): Stream filter convert.iconv.* is seekable only to start position in %s on line %d
Seek to middle: FAILURE
