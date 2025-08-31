--TEST--
DBA DBM handler test
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('dbm');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'dba_dbm.db';

$handler = 'dbm';
set_up_db($handler, $db_name, false /* Locking done by the library */);
run_standard_tests($handler, $db_name, false /* Locking done by the library */);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'dba_dbm.db';
cleanup_standard_db($db_name);
?>
--EXPECT--
key4: Another Content String
key2: Content String 2
key5: The last content string
Total keys: 3
Key 1 exists? N
Key 2 exists? Y
Key 3 exists? N
Key 4 exists? Y
Key 5 exists? Y
Content 2 replaced
Read during write: not allowed
Expected: Added a new data entry
Expected: Failed to insert data for already used key
Delete "key4"
Fetch "key2": Content 2 replaced 2nd time
Fetch "key number 6": The 6th value
array(3) {
  ["key number 6"]=>
  string(13) "The 6th value"
  ["key2"]=>
  string(27) "Content 2 replaced 2nd time"
  ["key5"]=>
  string(23) "The last content string"
}
