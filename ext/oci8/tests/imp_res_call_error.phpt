--TEST--
Oracle Database 12c Implicit Result Sets: using SQL 'CALL'
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
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

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "create or replace procedure imp_res_call_err_proc as
      c1 sys_refcursor;
    begin
      open c1 for select * from dual;
      dbms_sql.return_result(c1);
      open c1 for select * from dual;
      dbms_sql.return_result (c1);
    end;");

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "call imp_res_call_err_proc()");
oci_execute($s);

// Clean up

$stmtarray = array(
    "drop procedure imp_res_call_err_proc"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1

Warning: oci_execute(): ORA-29478: %s
ORA-06512: at "SYS.DBMS_SQL", line %d
ORA-06512: at "SYS.DBMS_SQL", line %d
ORA-06512: at "%s.IMP_RES_CALL_ERR_PROC", line %d in %simp_res_call_error.php on line %d
