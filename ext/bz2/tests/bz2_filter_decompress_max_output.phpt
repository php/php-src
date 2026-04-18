--TEST--
bzip2.decompress: max_output filter parameter
--EXTENSIONS--
bz2
--FILE--
<?php
$original = str_repeat('abcdefgh', 128); // 1024 bytes
$compressed = bzcompress($original);

echo "--- unbounded (no max_output) ---\n";
$fp = fopen('php://temp', 'w+');
stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_WRITE);
fwrite($fp, $compressed);
rewind($fp);
var_dump(strlen(stream_get_contents($fp)));
fclose($fp);

echo "--- max_output above actual size ---\n";
$fp = fopen('php://temp', 'w+');
stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_WRITE, ['max_output' => 2048]);
fwrite($fp, $compressed);
rewind($fp);
var_dump(strlen(stream_get_contents($fp)));
fclose($fp);

echo "--- max_output below actual size ---\n";
$fp = fopen('php://temp', 'w+');
stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_WRITE, ['max_output' => 100]);
fwrite($fp, $compressed);
rewind($fp);
var_dump(strlen(stream_get_contents($fp)) <= 100);
fclose($fp);

echo "--- max_output = 0 (invalid) ---\n";
$fp = fopen('php://temp', 'w+');
stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_WRITE, ['max_output' => 0]);
fclose($fp);

echo "--- max_output = -1 (invalid) ---\n";
$fp = fopen('php://temp', 'w+');
stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_WRITE, ['max_output' => -1]);
fclose($fp);
?>
--EXPECTF--
--- unbounded (no max_output) ---
int(1024)
--- max_output above actual size ---
int(1024)
--- max_output below actual size ---

Notice: fwrite(): bzip2.decompress: decompressed output exceeded max_output in %s on line %d
bool(true)
--- max_output = 0 (invalid) ---

Warning: stream_filter_append(): Invalid parameter given for max_output (0) in %s on line %d
--- max_output = -1 (invalid) ---

Warning: stream_filter_append(): Invalid parameter given for max_output (-1) in %s on line %d
