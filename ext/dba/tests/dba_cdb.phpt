--TEST--
DBA CDB handler test
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('cdb');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'dba_cdb.db';

set_up_cdb_db_and_run($db_name);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'dba_cdb.db';
cleanup_standard_db($db_name);
?>
--EXPECT--
Key 1 exists? Y
Key 2 exists? Y
Key 3 exists? Y
Key 4 exists? Y
Key 5 exists? Y
array(8) {
  ["[key10]name10"]=>
  string(17) "Content String 10"
  ["[key30]name30"]=>
  string(17) "Content String 30"
  ["key1"]=>
  string(16) "Content String 1"
  ["key2"]=>
  string(16) "Content String 2"
  ["key3"]=>
  string(20) "Third Content String"
  ["key4"]=>
  string(22) "Another Content String"
  ["key5"]=>
  string(23) "The last content string"
  ["name9"]=>
  string(16) "Content String 9"
}
--NO-LOCK--
Key 1 exists? Y
Key 2 exists? Y
Key 3 exists? Y
Key 4 exists? Y
Key 5 exists? Y
array(8) {
  ["[key10]name10"]=>
  string(17) "Content String 10"
  ["[key30]name30"]=>
  string(17) "Content String 30"
  ["key1"]=>
  string(16) "Content String 1"
  ["key2"]=>
  string(16) "Content String 2"
  ["key3"]=>
  string(20) "Third Content String"
  ["key4"]=>
  string(22) "Another Content String"
  ["key5"]=>
  string(23) "The last content string"
  ["name9"]=>
  string(16) "Content String 9"
}
