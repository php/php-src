--TEST--
Bug #14383 (8.0+) (using postgres with DBA causes DBA not to be able to find any keys)
--EXTENSIONS--
dba
pgsql
--SKIPIF--
<?php
require_once dirname(__DIR__, 2) . '/dba/tests/setup/setup_dba_tests.inc';
check_skip_any();
require_once('skipif.inc');
?>
--FILE--
<?php
require_once('config.inc');

$dbh = @pg_connect($conn_str);
if (!$dbh) {
    die ("Could not connect to the server");
}
pg_close($dbh);

require_once dirname(__DIR__, 2) . '/dba/tests/setup/setup_dba_tests.inc';
$name = 'bug14383.db';

$handler = get_any_handler($name);
run_standard_tests($handler, $name);
?>
--CLEAN--
<?php
require_once dirname(__DIR__, 2) . '/dba/tests/setup/setup_dba_tests.inc';
$name = 'bug14383.db';
cleanup_standard_db($name);
?>
--EXPECTF--
Using handler: "%s"
=== RUNNING WITH FILE LOCK ===
Remove key 1 and 3
bool(true)
bool(true)
Try to remove key 1 again
bool(false)
key2: Content String 2
key4: Another Content String
key5: The last content string
name9: Content String 9
[key10]name10: Content String 10
[key30]name30: Content String 30
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
=== RUNNING WITH DB LOCK (default) ===
SAME OUTPUT AS PREVIOUS RUN
=== RUNNING WITH NO LOCK ===
SAME OUTPUT AS PREVIOUS RUN (modulo read during write due to no lock)
