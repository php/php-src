--TEST--
Bug #44113 (New collection creation can fail with OCI-22303)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "create or replace type bug44113_list_t as table of number"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test
// The test can take some time to complete and can exceed PHP's test
// timeout limit on slow networks.

for ($x = 0; $x < 70000; $x++) {
    if (!($var = oci_new_collection($c, 'BUG44113_LIST_T'))) {
        print "Failed new collection creation on $x\n";
        break;
    }
}

print "Completed $x\n";

// Cleanup

$stmtarray = array(
    "drop type bug44113_list_t"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
Completed 70000
Done
