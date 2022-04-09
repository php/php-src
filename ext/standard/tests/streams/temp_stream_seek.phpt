--TEST--
BUG: php://temp does not preserve file-pointer once it switches from memory to temporary file
--FILE--
<?php

$f = fopen('php://temp/maxmemory:1024', 'r+');
fwrite($f, str_repeat("1", 738));
fseek($f, 0, SEEK_SET);
fwrite($f, str_repeat("2", 512));

fseek($f, 0, SEEK_SET);
var_dump(fread($f, 16));

fseek($f, 0, SEEK_END);
var_dump(ftell($f));

?>
--EXPECT--
string(16) "2222222222222222"
int(738)
