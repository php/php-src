--TEST--
Oracle Database 12c Implicit Result Sets: Commit modes
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
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

// Initialization

$c2 = oci_new_connect($user, $password, $dbase);

$stmtarray = array(
    "drop table imp_res_insert_tab",
    "create table imp_res_insert_tab (c1 number)",

    "create or replace procedure imp_res_insert_proc_nc (p1 in number) as
      c1 sys_refcursor;
    begin
      execute immediate 'insert into imp_res_insert_tab values ('||p1||')';
      open c1 for select * from imp_res_insert_tab order by 1;
      dbms_sql.return_result(c1);
    end;",

    "create or replace procedure imp_res_insert_proc_c (p1 in number) as
      c1 sys_refcursor;
    begin
      execute immediate 'insert into imp_res_insert_tab values ('||p1||')';
      commit;
      open c1 for select * from imp_res_insert_tab order by 1;
      dbms_sql.return_result(c1);
    end;"

);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1 - No commit in procedure, OCI_COMMIT_ON_SUCCESS mode\n";
$s = oci_parse($c, "begin imp_res_insert_proc_nc(111); end;");
oci_execute($s, OCI_COMMIT_ON_SUCCESS);
while (($row = oci_fetch_row($s)) !== false)
    echo $row[0], "\n";
$s2 = oci_parse($c2, "select * from imp_res_insert_tab order by 1");
oci_execute($s2, OCI_NO_AUTO_COMMIT);
oci_fetch_all($s2, $res);
var_dump($res['C1']);

echo "\nTest 2 - No commit in procedure, OCI_NO_AUTO_COMMIT mode\n";
$s = oci_parse($c, "begin imp_res_insert_proc_nc(222); end;");
oci_execute($s, OCI_NO_AUTO_COMMIT);
while (($row = oci_fetch_row($s)) !== false)
    echo $row[0], "\n";
// The 2nd connection won't see the newly inserted data
$s2 = oci_parse($c2, "select * from imp_res_insert_tab order by 1");
oci_execute($s2, OCI_NO_AUTO_COMMIT);
oci_fetch_all($s2, $res);
var_dump($res['C1']);

echo "\nTest 3 - Commit in procedure, OCI_COMMIT_ON_SUCCESS mode\n";
$s = oci_parse($c, "begin imp_res_insert_proc_c(333); end;");
oci_execute($s, OCI_COMMIT_ON_SUCCESS);
// The 2nd connection will now see the previously uncommitted data inserted in the previous test
while (($row = oci_fetch_row($s)) !== false)
    echo $row[0], "\n";
$s2 = oci_parse($c2, "select * from imp_res_insert_tab order by 1");
oci_execute($s2, OCI_NO_AUTO_COMMIT);
oci_fetch_all($s2, $res);
var_dump($res['C1']);

echo "\nTest 4 - Commit in procedure, OCI_NO_AUTO_COMMIT mode\n";
$s = oci_parse($c, "begin imp_res_insert_proc_c(444); end;");
oci_execute($s, OCI_NO_AUTO_COMMIT);
while (($row = oci_fetch_row($s)) !== false)
    echo $row[0], "\n";
$s2 = oci_parse($c2, "select * from imp_res_insert_tab order by 1");
oci_execute($s2, OCI_NO_AUTO_COMMIT);
oci_fetch_all($s2, $res);
var_dump($res['C1']);

// Clean up

$stmtarray = array(
    "drop procedure imp_res_insert_proc_nc",
    "drop procedure imp_res_insert_proc_c",
    "drop table imp_res_insert_tab",
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1 - No commit in procedure, OCI_COMMIT_ON_SUCCESS mode
111
array(1) {
  [0]=>
  string(3) "111"
}

Test 2 - No commit in procedure, OCI_NO_AUTO_COMMIT mode
111
222
array(1) {
  [0]=>
  string(3) "111"
}

Test 3 - Commit in procedure, OCI_COMMIT_ON_SUCCESS mode
111
222
333
array(3) {
  [0]=>
  string(3) "111"
  [1]=>
  string(3) "222"
  [2]=>
  string(3) "333"
}

Test 4 - Commit in procedure, OCI_NO_AUTO_COMMIT mode
111
222
333
444
array(4) {
  [0]=>
  string(3) "111"
  [1]=>
  string(3) "222"
  [2]=>
  string(3) "333"
  [3]=>
  string(3) "444"
}
===DONE===
