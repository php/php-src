--TEST--
Bug #36436 (DBA problem with Berkeley DB4)
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('db4');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'bug36436.db';

$db = set_up_db_ex('db4', $db_name, LockFlag::DbLock, persistent: true);

var_dump($db, dba_fetch('key1', $db));

var_dump(dba_firstkey($db));
var_dump(dba_nextkey($db));

dba_close($db);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'bug36436.db';
cleanup_standard_db($db_name);
?>
--EXPECTF--
resource(%d) of type (dba persistent)
string(16) "Content String 1"
string(13) "[key10]name10"
string(13) "[key30]name30"
