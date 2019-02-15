--TEST--
DBA dba.default_handler tests
--SKIPIF--
<?php
$handler = "flatfile";
require_once(dirname(__FILE__) .'/skipif.inc');
?>
--INI--
dba.default_handler=flatfile
--FILE--
<?php
$handler = "flatfile";
require_once(dirname(__FILE__) .'/test.inc');
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
require(dirname(__FILE__) .'/clean.inc');
?>
--EXPECTF--
database handler: flatfile
Test 1

Warning: ini_set(): No such handler: does_not_exist in %sdba012.php on line %d
resource(%d) of type (dba)
Test 2

Warning: dba_open(%stest0.dbm,n): No default handler selected in %sdba012.php on line %d
bool(false)
