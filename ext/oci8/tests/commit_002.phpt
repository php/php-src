--TEST--
Test oci_commit failure
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

// Initialization

$stmtarray = array(
    "drop table commit_002_tab",
    "create table commit_002_tab
     ( x int constraint commit_002_tab_check_x check ( x > 0 ) deferrable initially immediate,
       y int constraint commit_002_tab_check_y check ( y > 0 ) deferrable initially deferred)"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "First Insert\n";
$s = oci_parse($c, "insert into commit_002_tab values (-1, 1)");
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    $m = oci_error($s);
    echo 'Could not execute: '. $m['message'] . "\n";
}
$r = oci_commit($c);
if (!$r) {
    $m = oci_error($c);
    echo 'Could not commit: '. $m['message'] . "\n";
}


echo "Second Insert\n";
$s = oci_parse($c, "insert into commit_002_tab values (1, -1)");
$r = @oci_execute($s, OCI_NO_AUTO_COMMIT);
if (!$r) {
    $m = oci_error($s);
    echo 'Could not execute: '. $m['message'] . "\n";
}
$r = oci_commit($c);
if (!$r) {
    $m = oci_error($c);
    echo 'Could not commit: '. $m['message'] . "\n";
}


// Clean up

$stmtarray = array(
    "drop table commit_002_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
First Insert
Could not execute: ORA-02290: %s (%s.COMMIT_002_TAB_CHECK_X) %s
Second Insert

Warning: oci_commit(): ORA-02091: %s
ORA-02290: %s (%s.COMMIT_002_TAB_CHECK_Y) %s in %scommit_002.php on line %d
Could not commit: ORA-02091: %s
ORA-02290: %s (%s.COMMIT_002_TAB_CHECK_Y) %s
