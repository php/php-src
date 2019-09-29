--TEST--
Bug #69100: Bus error from stream_copy_to_stream (file -> SSL stream) with invalid length
--FILE--
<?php

$fileIn = __DIR__ . '/bug69100_in.txt';
$fileOut = __DIR__ . '/bug69100_out.txt';

file_put_contents($fileIn, str_repeat('A', 64 * 1024));
$fr = fopen($fileIn, 'rb');
$fw = fopen($fileOut, 'w');

var_dump(stream_copy_to_stream($fr, $fw, 32 * 1024));
var_dump(stream_copy_to_stream($fr, $fw, 64 * 1024));

fclose($fr);
fclose($fw);
unlink($fileIn);
unlink($fileOut);

?>
--EXPECT--
int(32768)
int(32768)
