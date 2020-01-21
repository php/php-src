--TEST--
Bug #79099 (OOB read in php_strip_tags_ex)
--FILE--
<?php
$stream = fopen('php://memory', 'w+');
fputs($stream, "<?\n\"\n");
rewind($stream);
var_dump(@fgetss($stream));
var_dump(@fgetss($stream));
fclose($stream);

$stream = fopen('php://memory', 'w+');
fputs($stream, "<\0\n!\n");
rewind($stream);
var_dump(@fgetss($stream));
var_dump(@fgetss($stream));
fclose($stream);

$stream = fopen('php://memory', 'w+');
fputs($stream, "<\0\n?\n");
rewind($stream);
var_dump(@fgetss($stream));
var_dump(@fgetss($stream));
fclose($stream);
?>
--EXPECT--
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
