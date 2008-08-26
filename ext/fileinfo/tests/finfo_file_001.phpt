--TEST--
finfo_file(): Testing file names
--FILE--
<?php

$fp = finfo_open();
var_dump(finfo_file($fp, "\0"));
var_dump(finfo_file($fp, ''));
var_dump(finfo_file($fp, NULL));
var_dump(finfo_file($fp, '.'));
var_dump(finfo_file($fp, '&'));

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
string(9) "directory"
bool(false)
