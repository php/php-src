--TEST--
bzip2.decompress filter with seek to start
--EXTENSIONS--
bz2
--FILE--
<?php
$file = __DIR__ . '/bz2_filter_seek_decompress.bz2';

$text = 'I am the very model of a modern major general, I\'ve information vegetable, animal, and mineral.';

$fp = fopen($file, 'w');
stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE);
fwrite($fp, $text);
fclose($fp);

$fp = fopen($file, 'r');
stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_READ);

$partial = fread($fp, 20);
echo "First read (20 bytes): " . $partial . "\n";

$result = fseek($fp, 0, SEEK_SET);
echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

$full = stream_get_contents($fp);
echo "Content after seek matches: " . ($full === $text ? "YES" : "NO") . "\n";

$result = fseek($fp, 50, SEEK_SET);
echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";

fclose($fp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/bz2_filter_seek_decompress.bz2');
?>
--EXPECTF--
First read (20 bytes): I am the very model 
Seek to start: SUCCESS
Content after seek matches: YES

Warning: fseek(): Stream filter bzip2.decompress is not seekable in %s on line %d
Seek to middle: FAILURE
