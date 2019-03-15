--TEST--
oci_set_db_operation: test DBOP for end-to-end tracing
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
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
if (!function_exists('oci_set_db_operation'))
{
    die("skip function oci_set_db_operation() does not exist");
}
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

function dq($c, $q)
{
    $s = oci_parse($c, $q);
    oci_execute($s);
    var_dump(oci_fetch_assoc($s));
}

echo "Test 1\n";
oci_set_db_operation($c, "db_op_2_a");
dq($c, 'select /*+ MONITOR */ * from dual');

oci_set_db_operation($c, "db_op_2_b");
dq($c, 'select /*+ MONITOR */ * from dual');

dq($c, 'select dbop_name from v$sql_monitor where dbop_name like \'db_op_2%\' order by dbop_exec_id desc');

echo "Test 2\n";
oci_set_db_operation($c, "");
dq($c, 'select /*+ MONITOR */ \'dboptest\' from dual');

dq($c, 'select sql_text, dbop_name from v$sql_monitor where sql_text like \'%dboptest%\' order by dbop_exec_id desc');

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
  ["DUMMY"]=>
  string(1) "X"
}
array(1) {
  ["DBOP_NAME"]=>
  string(9) "db_op_2_b"
}
Test 2
array(1) {
  ["'DBOPTEST'"]=>
  string(8) "dboptest"
}
array(2) {
  ["SQL_TEXT"]=>
  string(42) "select /*+ MONITOR */ 'dboptest' from dual"
  ["DBOP_NAME"]=>
  NULL
}
===DONE===
