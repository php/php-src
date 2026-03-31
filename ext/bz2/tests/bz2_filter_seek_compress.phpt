--TEST--
bzip2.compress filter with seek to start
--EXTENSIONS--
bz2
--FILE--
<?php
$file = __DIR__ . '/bz2_filter_seek_compress.bz2';

$text1 = 'Short text.';
$text2 = 'This is a much longer text that will completely overwrite the previous compressed data in the file.';

$fp = fopen($file, 'w+');
stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE);

fwrite($fp, $text1);
fflush($fp);

$size1 = ftell($fp);
echo "Size after first write: $size1\n";

$result = fseek($fp, 0, SEEK_SET);
echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

fwrite($fp, $text2);
fflush($fp);

$size2 = ftell($fp);
echo "Size after second write: $size2\n";
echo "Second write is larger: " . ($size2 > $size1 ? "YES" : "NO") . "\n";

$result = fseek($fp, 50, SEEK_SET);
echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

fclose($fp);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_READ);
$content = stream_get_contents($fp);
fclose($fp);

echo "Decompressed content matches text2: " . ($content === $text2 ? "YES" : "NO") . "\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/bz2_filter_seek_compress.bz2');
?>
--EXPECTF--
Size after first write: 40
Seek to start: SUCCESS
Size after second write: 98
Second write is larger: YES

Warning: fseek(): Stream filter bzip2.compress is seekable only to start position in %s on line %d
Seek to middle: FAILURE
Decompressed content matches text2: YES
