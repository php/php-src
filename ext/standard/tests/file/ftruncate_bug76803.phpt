--TEST--
Bug #76803 ftruncate changes file pointer
--FILE--
<?php

$fn = __DIR__ . DIRECTORY_SEPARATOR . "test76803";

$f = fopen($fn, "w");
fwrite($f, "Hello");
ftruncate($f, 2);
fwrite($f, "World");
fclose($f);
var_dump(addslashes(file_get_contents($fn)));

$f = fopen($fn, "w");
fwrite($f, "Hello");
ftruncate($f, 2);
fclose($f);
var_dump(addslashes(file_get_contents($fn)));

$f = fopen('php://memory', 'w+');
fwrite($f, 'Hello');
ftruncate($f, 2); // in 7.3 changes file pointer to 2
fwrite($f, 'World');
rewind($f);
var_dump(addslashes(stream_get_contents($f)));
fclose($f);

?>
--CLEAN--
<?php
$fn = __DIR__ . DIRECTORY_SEPARATOR . "test76803";
unlink($fn);
?>
--EXPECT--
string(13) "He\0\0\0World"
string(2) "He"
string(7) "HeWorld"
