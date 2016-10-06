--TEST--
Bug #68948: feof() on temporary streams broken
--FILE--
<?php

$testString = '0123456789';

$stream = fopen("php://memory", "r+");
fwrite($stream, $testString);
rewind($stream);

var_dump(ftell($stream));
var_dump(feof($stream));

var_dump(fread($stream, 1024));

var_dump(ftell($stream));
var_dump(feof($stream));

var_dump(fread($stream, 1024));

var_dump(ftell($stream));
var_dump(feof($stream));

?>
--EXPECT--
int(0)
bool(false)
string(10) "0123456789"
int(10)
bool(true)
string(0) ""
int(10)
bool(true)
