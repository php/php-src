--TEST--
Bug #76857 (Can read "non-existant" files)
--FILE--
<?php
file_put_contents(__DIR__ . '/bug76857_data.txt', 'test data');
$path = "foobar://google.com/../../bug76857_data.txt";
chdir(__DIR__);
var_dump(file_exists($path));
var_dump(file_get_contents($path, false, null, 0, 10));
?>
--EXPECTF--
Warning: file_exists(): Unable to find the wrapper "foobar" - did you forget to enable it when you configured PHP? in %s on line %d
bool(true)

Warning: file_get_contents(): Unable to find the wrapper "foobar" - did you forget to enable it when you configured PHP? in %s on line %d
string(9) "test data"
--CLEAN--
<?php
@unlink(__DIR__ . '/bug76857_data.txt');
?>