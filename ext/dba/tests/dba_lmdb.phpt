--TEST--
DBA LMDB handler test
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('lmdb');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'dba_lmdb.db';

$handler = 'lmdb';
run_standard_tests($handler, $db_name);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'dba_lmdb.db';
cleanup_standard_db($db_name);
?>
--EXPECTF--
=== RUNNING WITH FILE LOCK ===

Notice: dba_open(): Handler lmdb does locking internally in %s on line %d

Notice: dba_open(): Handler lmdb does locking internally in %s on line %d

Notice: dba_open(): Handler lmdb does locking internally in %s on line %d
Remove key 1 and 3
bool(true)
bool(true)
Try to remove key 1 again

Warning: dba_delete(): MDB_NOTFOUND: No matching key/data pair found in %s on line %d
bool(false)
[key10]name10: Content String 10
[key30]name30: Content String 30
key2: Content String 2
key4: Another Content String
key5: The last content string
name9: Content String 9
Total keys: 6
Key 1 exists? N
Key 2 exists? Y
Key 3 exists? N
Key 4 exists? Y
Key 5 exists? Y
Replace second key data
bool(true)
Content 2 replaced
Read during write: not allowed
Expected: Added a new data entry
Expected: Failed to insert data for already used key
Replace second key data
bool(true)
Delete "key4"
bool(true)
Fetch "key2": Content 2 replaced 2nd time
Fetch "key number 6": The 6th value

Notice: dba_open(): Handler lmdb does locking internally in %s on line %d
array(6) {
  ["[key10]name10"]=>
  string(17) "Content String 10"
  ["[key30]name30"]=>
  string(17) "Content String 30"
  ["key number 6"]=>
  string(13) "The 6th value"
  ["key2"]=>
  string(27) "Content 2 replaced 2nd time"
  ["key5"]=>
  string(23) "The last content string"
  ["name9"]=>
  string(16) "Content String 9"
}
=== RUNNING WITH DB LOCK (default) ===
Remove key 1 and 3
bool(true)
bool(true)
Try to remove key 1 again

Warning: dba_delete(): MDB_NOTFOUND: No matching key/data pair found in %s on line %d
bool(false)
[key10]name10: Content String 10
[key30]name30: Content String 30
key2: Content String 2
key4: Another Content String
key5: The last content string
name9: Content String 9
Total keys: 6
Key 1 exists? N
Key 2 exists? Y
Key 3 exists? N
Key 4 exists? Y
Key 5 exists? Y
Replace second key data
bool(true)
Content 2 replaced
Read during write: not allowed
Expected: Added a new data entry
Expected: Failed to insert data for already used key
Replace second key data
bool(true)
Delete "key4"
bool(true)
Fetch "key2": Content 2 replaced 2nd time
Fetch "key number 6": The 6th value
array(6) {
  ["[key10]name10"]=>
  string(17) "Content String 10"
  ["[key30]name30"]=>
  string(17) "Content String 30"
  ["key number 6"]=>
  string(13) "The 6th value"
  ["key2"]=>
  string(27) "Content 2 replaced 2nd time"
  ["key5"]=>
  string(23) "The last content string"
  ["name9"]=>
  string(16) "Content String 9"
}
=== RUNNING WITH NO LOCK ===

Warning: dba_open(): Locking cannot be disabled for handler lmdb in %s on line %d
Failed to create DB
