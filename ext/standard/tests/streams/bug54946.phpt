--TEST--
Bug#54946 stream_get_contents infinite loop
--FILE--
<?php
$filename = tempnam(sys_get_temp_dir(), "phpbug");

$stream = fopen($filename, "w"); // w or a
$retval = stream_get_contents($stream, 1, 1);

var_dump($retval);
unlink($filename);



$filename = tempnam(sys_get_temp_dir(), "phpbug2");

$stream = fopen($filename, "a");
$retval = stream_get_contents($stream, 1, 1);

var_dump($retval);
unlink($filename);



$filename = tempnam(sys_get_temp_dir(), "phpbug3");

$stream = fopen($filename, "a");
fseek($stream, 1);
$retval = stream_get_contents($stream, 1);

var_dump($retval);
unlink($filename);
?>
===DONE===
--EXPECT--
string(0) ""
string(0) ""
string(0) ""
===DONE===

