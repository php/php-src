--TEST--
Oracle Database 12c Implicit Result Sets: alternating oci_fetch_* calls
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
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
    "drop table imp_res_6_tab",
    "create table imp_res_6_tab (c1 number, c2 varchar2(10))",
    "insert into imp_res_6_tab values (1, 'a')",
    "insert into imp_res_6_tab values (2, 'b')",
    "insert into imp_res_6_tab values (3, 'c')",
    "insert into imp_res_6_tab values (4, 'd')",
    "insert into imp_res_6_tab values (5, 'e')",
    "insert into imp_res_6_tab values (6, 'f')",

    "create or replace procedure imp_res_6_proc as
      c1 sys_refcursor;
    begin
      open c1 for select * from imp_res_6_tab order by 1;
      dbms_sql.return_result(c1);
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";
$s = oci_parse($c, "begin imp_res_6_proc(); end;");
oci_execute($s);

$row = oci_fetch_assoc($s);
var_dump($row);
$row = oci_fetch_row($s);
var_dump($row);
$row = oci_fetch_object($s);
var_dump($row);
$row = oci_fetch_array($s);
var_dump($row);
$row = oci_fetch_array($s, OCI_NUM);
var_dump($row);
$row = oci_fetch_array($s, OCI_ASSOC);
var_dump($row);


// Clean up

$stmtarray = array(
    "drop procedure imp_res_6_proc",
    "drop table imp_res_6_tab",
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
array(2) {
  ["C1"]=>
  string(1) "1"
  ["C2"]=>
  string(1) "a"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(1) "b"
}
object(stdClass)#%d (2) {
  ["C1"]=>
  string(1) "3"
  ["C2"]=>
  string(1) "c"
}
array(4) {
  [0]=>
  string(1) "4"
  ["C1"]=>
  string(1) "4"
  [1]=>
  string(1) "d"
  ["C2"]=>
  string(1) "d"
}
array(2) {
  [0]=>
  string(1) "5"
  [1]=>
  string(1) "e"
}
array(2) {
  ["C1"]=>
  string(1) "6"
  ["C2"]=>
  string(1) "f"
}
===DONE===
