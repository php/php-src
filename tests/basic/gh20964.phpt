--TEST--
GH-20964 (fseek() on php://memory with PHP_INT_MIN causes undefined behavior)
--FILE--
<?php
$stream = fopen('php://memory', 'r+');
fwrite($stream, 'hello');
rewind($stream);

var_dump(fseek($stream, PHP_INT_MIN, SEEK_END));
var_dump(ftell($stream));

rewind($stream);
var_dump(fseek($stream, PHP_INT_MIN, SEEK_CUR));
var_dump(ftell($stream));

fclose($stream);
?>
--EXPECT--
int(-1)
bool(false)
int(-1)
bool(false)
