--TEST--
DBA check behaviour of array keys (inifile version)
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('inifile');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$name = __DIR__ . '/inifile_group_keys_tests.db';

$db = dba_open($name, 'c-', 'inifile');

echo "Checks on an empty database:\n";
var_dump(dba_delete(['group', 'name'], $db));
var_dump(dba_replace(['group', 'name'], 'Normal group', $db));
var_dump(dba_exists(['group', 'name'], $db));
var_dump(dba_fetch(['group', 'name'], $db));
var_dump(dba_delete(['group', 'name'], $db));

echo "Insert various group values:\n";
var_dump(dba_insert(['group1', 'name1'], 'Group1Value1', $db));
var_dump(dba_insert(['group1', 'name2'], 'Group1Value2', $db));
var_dump(dba_insert(['group1', 'name3'], 'Group1Value3', $db));
var_dump(dba_insert(['group2', 'name1'], 'Group1Value1', $db));
var_dump(dba_insert(['group2', 'name2'], 'Group1Value2', $db));

$INI_FILE_CONTENT_AT_STAGE = <<<'INIFILE'
[group]
[group1]
name1=Group1Value1
name2=Group1Value2
name3=Group1Value3
[group2]
name1=Group1Value1
name2=Group1Value2

INIFILE;

echo "Check INI file content:\n";
var_dump(file_get_contents($name) == $INI_FILE_CONTENT_AT_STAGE);

echo "Insert an existing value in group1, this should fail\n";
// TODO This is bugged
var_dump(dba_insert(['group1', 'name2'], 'Group1Value2reinserted', $db));
var_dump(dba_fetch(['group1', 'name2'], $db));

$INI_FILE_CONTENT_AT_STAGE = <<<'INIFILE'
[group]
[group1]
name1=Group1Value1
name2=Group1Value2
name3=Group1Value3
name2=Group1Value2reinserted
[group2]
name1=Group1Value1
name2=Group1Value2

INIFILE;
echo "Check INI file content:\n";
var_dump(file_get_contents($name) == $INI_FILE_CONTENT_AT_STAGE);

echo "Insert a new value in group1\n";
var_dump(dba_insert(['group1', 'name4'], 'Group1Value4', $db));
var_dump(dba_fetch(['group1', 'name4'], $db));

$INI_FILE_CONTENT_AT_STAGE = <<<'INIFILE'
[group]
[group1]
name1=Group1Value1
name2=Group1Value2
name3=Group1Value3
name2=Group1Value2reinserted
name4=Group1Value4
[group2]
name1=Group1Value1
name2=Group1Value2

INIFILE;
echo "Check INI file content:\n";
var_dump(file_get_contents($name) == $INI_FILE_CONTENT_AT_STAGE);

// TODO This is also bugged
echo "Replace value of ['group1', 'name3'] with 'Group1Value3replaced'\n";
var_dump(dba_insert(['group1', 'name3'], 'Group1Value3replaced', $db));
var_dump(dba_fetch(['group1', 'name3'], $db));

$INI_FILE_CONTENT_AT_STAGE = <<<'INIFILE'
[group]
[group1]
name1=Group1Value1
name2=Group1Value2
name3=Group1Value3
name2=Group1Value2reinserted
name4=Group1Value4
name3=Group1Value3replaced
[group2]
name1=Group1Value1
name2=Group1Value2

INIFILE;
echo "Check INI file content:\n";
var_dump(file_get_contents($name) == $INI_FILE_CONTENT_AT_STAGE);

dba_close($db);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$name = __DIR__ . '/inifile_group_keys_tests.db';
cleanup_standard_db($name);
?>
--EXPECT--
Checks on an empty database:
bool(false)
bool(true)
bool(true)
string(12) "Normal group"
bool(true)
Insert various group values:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Check INI file content:
bool(true)
Insert an existing value in group1, this should fail
bool(true)
string(12) "Group1Value2"
Check INI file content:
bool(true)
Insert a new value in group1
bool(true)
string(12) "Group1Value4"
Check INI file content:
bool(true)
Replace value of ['group1', 'name3'] with 'Group1Value3replaced'
bool(true)
string(12) "Group1Value3"
Check INI file content:
bool(true)
