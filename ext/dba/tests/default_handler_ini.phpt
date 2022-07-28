--TEST--
DBA dba.default_handler tests
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('flatfile');
?>
--INI--
dba.default_handler=flatfile
--FILE--
<?php
$handler = "flatfile";
$db_filename = 'ini_test_default_handler.db';
echo "database handler: $handler\n";

echo "Test 1\n";

ini_set('dba.default_handler', 'does_not_exist');

var_dump(dba_open($db_filename, 'c'));

echo "Test 2\n";

ini_set('dba.default_handler', '');

var_dump(dba_open($db_filename, 'n'));

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'ini_test_default_handler.db';
cleanup_standard_db($db_name);
?>
--EXPECTF--
database handler: flatfile
Test 1

Warning: ini_set(): No such handler: does_not_exist in %s on line %d
resource(%d) of type (dba)
Test 2

Warning: dba_open(): No default handler selected in %s on line %d
bool(false)
