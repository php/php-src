--TEST--
Oracle Database 12c Implicit Result Sets: oci_fetch_all
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
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

require __DIR__.'/connect.inc';

// Initialization

$stmtarray = array(
    "create or replace procedure imp_res_5_proc as
      c1 sys_refcursor;
    begin
        open c1 for select 1 from dual union select 2 from dual;
        dbms_sql.return_result (c1);
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";
$s = oci_parse($c, "begin imp_res_5_proc(); end;");
oci_execute($s);
oci_fetch_all($s,$res);  // This will fail with ORA-24374
var_dump($res);

echo "\nTest 2\n";
$s = oci_parse($c, "begin imp_res_5_proc(); end;");
oci_execute($s);
$r = oci_fetch_row($s);
var_dump($r);
oci_fetch_all($s, $res);  // This will fail with ORA-24374
var_dump($res);
$r = oci_fetch_row($s);
var_dump($r);

// Clean up

$stmtarray = array(
    "drop procedure imp_res_5_proc",
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1

Warning: oci_fetch_all(): ORA-24374: %s in %simp_res_5.php on line %d
array(0) {
}

Test 2
array(1) {
  [0]=>
  string(1) "1"
}

Warning: oci_fetch_all(): ORA-24374: %s in %simp_res_5.php on line %d
array(0) {
}
array(1) {
  [0]=>
  string(1) "2"
}
