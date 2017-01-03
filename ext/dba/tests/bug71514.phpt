--TEST--
Bug #71514 (Bad dba_replace condition because of wrong API usage)
--SKIPIF--
<?php
if (!extension_loaded('dba')) die('skip dba extension not available');
if (!in_array('inifile', dba_handlers())) die('skip inifile handler not available');
?>
--FILE--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'bug71514.ini';

$db = dba_open($filename, 'c', 'inifile');

dba_insert('foo', 'value1', $db);
dba_replace('foo', 'value2', $db);
var_dump(dba_fetch('foo', $db));

dba_close($db);
?>
==DONE==
--EXPECT--
string(6) "value2"
==DONE==
--CLEAN--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'bug71514.ini';
unlink($filename);
?>
