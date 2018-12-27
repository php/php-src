--TEST--
oci_set_db_operation: basic test for end-to-end tracing
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) {
    die("skip needs to be run as a DBA user");
}
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 12)) {
    die("skip expected output only valid when using Oracle Database 12c or greater");
}
preg_match('/^[[:digit:]]+/', oci_client_version(), $matches);
if (!(isset($matches[0]) && $matches[0] >= 12)) {
    die("skip works only with Oracle 12c or greater version of Oracle client libraries");
}
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Run Test

echo "Test 1\n";

// Test setting the "DB operation" used by Oracle DB for end-to-end application tracing

function dq($c, $q)
{
    $s = oci_parse($c, $q);
    oci_execute($s);
    var_dump(oci_fetch_assoc($s));
}

oci_set_db_operation($c, "db_op_1");
dq($c, 'select /*+ MONITOR */ * from dual');

dq($c, 'select dbop_name from v$sql_monitor where dbop_name is not null order by dbop_exec_id desc');

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1
array(1) {
  ["DUMMY"]=>
  string(1) "X"
}
array(1) {
  ["DBOP_NAME"]=>
  string(7) "db_op_1"
}
===DONE===
