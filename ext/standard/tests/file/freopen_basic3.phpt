--TEST--
basic freopen() test with errors - 2
--FILE--
<?php

$filename = dirname(__FILE__)."/freopen_basic3.tmp";
freopen("", "w", STDIN);
freopen($filename, "test", STDIN);

$fp = fopen($filename, "w");
var_dump(freopen($filename, "r", STDERR));

var_dump(fwrite($fp, "test"));
var_dump(fread(STDERR, 100));

var_dump(freopen($filename, "a", STDERR));

var_dump(fwrite($fp, "test2"));
var_dump(fread(STDERR, 100));

?>
--CLEAN--
<?php
$filename = dirname(__FILE__)."/freopen_basic3.tmp";
unlink($filename);
?>
--EXPECTF--
Warning: freopen(): Filename cannot be empty in %s on line %d

Warning: freopen(): `test' is not a valid mode for freopen in %s on line %d
bool(true)
int(4)
string(4) "test"
bool(true)
int(5)
string(0) ""
