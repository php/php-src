--TEST--
Oracle Database 12c Implicit Result Sets: Zero Rows
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
    "create or replace procedure imp_res_2_proc_a as
      c1 sys_refcursor;
    begin
      open c1 for select * from dual where 1 = 0;
      dbms_sql.return_result(c1);
    end;",

    "create or replace procedure imp_res_2_proc_b as
      c1 sys_refcursor;
    begin
      open c1 for select * from dual;
      dbms_sql.return_result(c1);
      open c1 for select * from dual where 1 = 0;
      dbms_sql.return_result(c1);
    end;",

    "create or replace procedure imp_res_2_proc_c as
      c1 sys_refcursor;
    begin
      open c1 for select * from dual where 1 = 0;
      dbms_sql.return_result(c1);
      open c1 for select * from dual;
      dbms_sql.return_result(c1);
    end;"

);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";
$s = oci_parse($c, "begin imp_res_2_proc_a(); end;");
oci_execute($s);
while (($row = oci_fetch_row($s)) != false)
    var_dump($row);

echo "Test 2\n";
$s = oci_parse($c, "begin imp_res_2_proc_b(); end;");
oci_execute($s);
while (($row = oci_fetch_row($s)) != false)
    var_dump($row);

echo "Test 2\n";
$s = oci_parse($c, "begin imp_res_2_proc_c(); end;");
oci_execute($s);
while (($row = oci_fetch_row($s)) != false)
    var_dump($row);

// Clean up

$stmtarray = array(
    "drop procedure imp_res_2_proc_a",
    "drop procedure imp_res_2_proc_b",
    "drop procedure imp_res_2_proc_c"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECT--
Test 1
Test 2
array(1) {
  [0]=>
  string(1) "X"
}
Test 2
array(1) {
  [0]=>
  string(1) "X"
}
