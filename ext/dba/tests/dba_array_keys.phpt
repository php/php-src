--TEST--
DBA check behaviour of array keys
--EXTENSIONS--
dba
--SKIPIF--
<?php
    require_once(__DIR__ .'/skipif.inc');
    die("info $HND handler used");
?>
--FILE--
<?php
require_once(__DIR__ .'/test.inc');
$db_file = dba_open($db_file, "n", $handler);

if ($db_file === false) {
    die('Error creating database');
}

var_dump(dba_insert(['group', 'name'], 'Normal group', $db_file));
var_dump(dba_insert(['group', ''], 'Empty name', $db_file));
var_dump(dba_insert(['', 'name'], 'Empty group', $db_file));
var_dump(dba_insert(['', ''], 'Empty keys', $db_file));
var_dump(dba_fetch(['group', 'name'], $db_file));
var_dump(dba_fetch(['group', ''], $db_file));
var_dump(dba_fetch(['', 'name'], $db_file));
var_dump(dba_fetch(['', ''], $db_file));
dba_close($db_file);

?>
--CLEAN--
<?php
    require(__DIR__ .'/clean.inc');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
string(12) "Normal group"
string(10) "Empty name"
string(11) "Empty group"
string(10) "Empty keys"
