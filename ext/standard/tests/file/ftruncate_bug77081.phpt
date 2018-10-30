--TEST--
Bug #77081 ftruncate() changes seek pointer in c mode
--FILE--
<?php

$filename = dirname(__FILE__) . DIRECTORY_SEPARATOR . "test77081";

file_put_contents($filename, 'foo');
$stream = fopen($filename, 'c');
ftruncate($stream, 0);
var_dump(ftell($stream));
fwrite($stream, 'bar');
fclose($stream);
var_dump(file_get_contents($filename));

?>
--CLEAN--
<?php
$fn = dirname(__FILE__) . DIRECTORY_SEPARATOR . "test77081";
unlink($fn);
?>
--EXPECT--
int(0)
string(3) "bar"
