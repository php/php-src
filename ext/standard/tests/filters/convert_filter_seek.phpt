--TEST--
convert filters (base64, quoted-printable) with seek to start only
--FILE--
<?php
$file = __DIR__ . '/convert_filters_seek.txt';

$text = 'Hello World! This is a test for convert filter seeking functionality.';

echo "Testing convert.base64-encode/decode\n";
$fp = fopen($file, 'w');
stream_filter_append($fp, 'convert.base64-encode', STREAM_FILTER_WRITE);
fwrite($fp, $text);
fclose($fp);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'convert.base64-decode', STREAM_FILTER_READ);

$partial = fread($fp, 20);
echo "First read (20 bytes): $partial\n";

$result = fseek($fp, 0, SEEK_SET);
echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

$full = fread($fp, strlen($text));
echo "Content matches: " . ($full === $text ? "YES" : "NO") . "\n";

$result = fseek($fp, 50, SEEK_SET);
echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

fclose($fp);

echo "\nTesting convert.quoted-printable-encode/decode\n";
$text2 = "Line1\r\nLine2\r\nLine3";
$fp = fopen($file, 'w');
stream_filter_append($fp, 'convert.quoted-printable-encode', STREAM_FILTER_WRITE);
fwrite($fp, $text2);
fclose($fp);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'convert.quoted-printable-decode', STREAM_FILTER_READ);

$partial = fread($fp, 10);
echo "First read (10 bytes): " . bin2hex($partial) . "\n";

$result = fseek($fp, 0, SEEK_SET);
echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

$full = fread($fp, strlen($text2));
echo "Content matches: " . ($full === $text2 ? "YES" : "NO") . "\n";

$result = fseek($fp, 20, SEEK_SET);
echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

fclose($fp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/convert_filters_seek.txt');
?>
--EXPECTF--
Testing convert.base64-encode/decode
First read (20 bytes): Hello World! This is
Seek to start: SUCCESS
Content matches: YES

Warning: fseek(): Stream filter convert.* is seekable only to start position in %s on line %d
Seek to middle: FAILURE

Testing convert.quoted-printable-encode/decode
First read (10 bytes): 4c696e65310d0a4c696e
Seek to start: SUCCESS
Content matches: YES

Warning: fseek(): Stream filter convert.* is seekable only to start position in %s on line %d
Seek to middle: FAILURE
