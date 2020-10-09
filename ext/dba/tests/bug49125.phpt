--TEST--
Bug #49125 (Error in dba_exists C code)
--SKIPIF--
<?php
    $handler = 'db4';
    require_once(__DIR__ .'/skipif.inc');
?>
--FILE--
<?php

error_reporting(E_ALL);

$handler = 'db4';
require_once(__DIR__ .'/test.inc');

$db = dba_popen($db_filename, 'c', 'db4');

dba_insert('foo', 'foo', $db);

var_dump(dba_exists('foo', $db));

dba_close($db);

?>
--CLEAN--
<?php
    require(__DIR__ .'/clean.inc');
?>
--EXPECT--
bool(true)
