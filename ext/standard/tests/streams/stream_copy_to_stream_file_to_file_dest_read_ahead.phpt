--TEST--
stream_copy_to_stream() file to file with a partially read destination
--FILE--
<?php

$srcFile = __DIR__ . '/stream_copy_dest_read_ahead_src.txt';
$dstFile = __DIR__ . '/stream_copy_dest_read_ahead_dst.txt';

file_put_contents($srcFile, str_repeat("N", 50));
file_put_contents($dstFile, str_repeat("O", 3000));

$src = fopen($srcFile, 'r');
$dst = fopen($dstFile, 'r+');
/* Buffered read-ahead moves the fd offset past the stream position; the copy
 * must land at the stream position. */
fread($dst, 10);

$copied = stream_copy_to_stream($src, $dst);
var_dump($copied);
var_dump(ftell($dst));

fclose($src);
fclose($dst);

$result = file_get_contents($dstFile);
var_dump(strlen($result));
var_dump($result === str_repeat("O", 10) . str_repeat("N", 50) . str_repeat("O", 2940));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stream_copy_dest_read_ahead_src.txt');
@unlink(__DIR__ . '/stream_copy_dest_read_ahead_dst.txt');
?>
--EXPECT--
int(50)
int(60)
int(3000)
bool(true)
