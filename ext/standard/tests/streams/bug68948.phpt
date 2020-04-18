--TEST--
Bug #68948: feof() on temporary streams broken
--FILE--
<?php

$testString = '0123456789';

$stream = fopen("php://memory", "r+");
fwrite($stream, $testString);
rewind($stream);

var_dump(fread($stream, 10));
var_dump(ftell($stream));
var_dump(feof($stream));

rewind($stream);

var_dump(fread($stream, 11));
var_dump(ftell($stream));
var_dump(feof($stream));

?>
--EXPECT--
string(10) "0123456789"
int(10)
bool(false)
string(10) "0123456789"
int(10)
bool(true)
