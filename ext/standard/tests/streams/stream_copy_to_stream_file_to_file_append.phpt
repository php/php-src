--TEST--
stream_copy_to_stream() file to file with an append-mode destination
--FILE--
<?php

$srcFile = __DIR__ . '/stream_copy_append_src.txt';
$dstFile = __DIR__ . '/stream_copy_append_dst.txt';

file_put_contents($srcFile, str_repeat("b", 3000));
file_put_contents($dstFile, "PREFIX-");

$src = fopen($srcFile, 'r');
/* O_APPEND must disable the fd-level copy fast-path and still append correctly. */
$dst = fopen($dstFile, 'a');

$copied = stream_copy_to_stream($src, $dst);
var_dump($copied);

fclose($src);
fclose($dst);

$result = file_get_contents($dstFile);
var_dump(strlen($result));
var_dump($result === "PREFIX-" . str_repeat("b", 3000));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stream_copy_append_src.txt');
@unlink(__DIR__ . '/stream_copy_append_dst.txt');
?>
--EXPECT--
int(3000)
int(3007)
bool(true)
