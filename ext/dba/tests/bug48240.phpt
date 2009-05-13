--TEST--
Bug #48240 (DBA Segmentation fault dba_nextkey)
--SKIPIF--
<?php 
	$handler = 'db4';
	require_once('skipif.inc');
?>
--FILE--
<?php

$handler = 'db4';
require_once('test.inc');

$db = dba_open($db_filename, 'c', 'db4');

var_dump(dba_nextkey($db));

dba_close($db);
unlink($db_filename);

?>
===DONE===
--EXPECT--
bool(false)
===DONE===
