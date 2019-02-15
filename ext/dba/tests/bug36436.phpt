--TEST--
Bug #36436 (DBA problem with Berkeley DB4)
--SKIPIF--
<?php
	$handler = 'db4';
	require_once(dirname(__FILE__) .'/skipif.inc');
?>
--FILE--
<?php

$handler = 'db4';
require_once(dirname(__FILE__) .'/test.inc');

$db = dba_popen($db_filename, 'c', 'db4');

dba_insert('X', 'XYZ', $db);
dba_insert('Y', '123', $db);

var_dump($db, dba_fetch('X', $db));

var_dump(dba_firstkey($db));
var_dump(dba_nextkey($db));

dba_close($db);

?>
===DONE===
--CLEAN--
<?php
	require(dirname(__FILE__) .'/clean.inc');
?>
--EXPECTF--
resource(%d) of type (dba persistent)
string(3) "XYZ"
string(1) "X"
string(1) "Y"
===DONE===
