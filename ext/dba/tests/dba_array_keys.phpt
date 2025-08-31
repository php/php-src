--TEST--
DBA check behaviour of array keys
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip_any();
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$name = 'array_keys_basic.db';

$db = get_any_db($name);

var_dump(dba_insert(['group', 'name'], 'Normal group', $db));
var_dump(dba_insert(['group', ''], 'Empty name', $db));
var_dump(dba_insert(['', 'name'], 'Empty group', $db));
var_dump(dba_insert(['', ''], 'Empty keys', $db));
var_dump(dba_fetch(['group', 'name'], $db));
var_dump(dba_fetch(['group', ''], $db));
var_dump(dba_fetch(['', 'name'], $db));
var_dump(dba_fetch(['', ''], $db));
dba_close($db);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'array_keys_basic.db';
cleanup_standard_db($db_name);
?>
--EXPECTF--
Using handler: "%s"
bool(true)
bool(true)
bool(true)
bool(true)
string(12) "Normal group"
string(10) "Empty name"
string(11) "Empty group"
string(10) "Empty keys"
