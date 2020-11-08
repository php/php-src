--TEST--
Oracle Database 12c Implicit Result Sets: oci_get_implicit_resultset: similar to imp_res_get_1 but with unrolled loop
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
    "drop table imp_res_get_2_tab_1",
    "create table imp_res_get_2_tab_1 (c1 number, c2 varchar2(10))",
    "insert into imp_res_get_2_tab_1 values (1, 'abcde')",
    "insert into imp_res_get_2_tab_1 values (2, 'fghij')",
    "insert into imp_res_get_2_tab_1 values (3, 'klmno')",

    "drop table imp_res_get_2_tab_2",
    "create table imp_res_get_2_tab_2 (c3 varchar2(1))",
    "insert into imp_res_get_2_tab_2 values ('t')",
    "insert into imp_res_get_2_tab_2 values ('u')",
    "insert into imp_res_get_2_tab_2 values ('v')",

    "create or replace procedure imp_res_get_2_proc as
      c1 sys_refcursor;
    begin
      open c1 for select * from imp_res_get_2_tab_1 order by 1;
      dbms_sql.return_result(c1);

      open c1 for select * from imp_res_get_2_tab_2 where rownum < 3 order by 1;
      dbms_sql.return_result(c1);

      open c1 for select * from dual;
      dbms_sql.return_result (c1);
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "begin imp_res_get_2_proc(); end;");
oci_execute($s);

$s1 = oci_get_implicit_resultset($s);
while (($row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS))) {
    foreach ($row as $item) {
        echo "  ".$item;
    }
    echo "\n";
}

$s2 = oci_get_implicit_resultset($s);
while (($row = oci_fetch_array($s2, OCI_ASSOC+OCI_RETURN_NULLS))) {
    foreach ($row as $item) {
        echo "  ".$item;
    }
    echo "\n";
}
oci_free_statement($s2);

$s3 = oci_get_implicit_resultset($s);
while (($row = oci_fetch_array($s3, OCI_ASSOC+OCI_RETURN_NULLS))) {
    foreach ($row as $item) {
        echo "  ".$item;
    }
    echo "\n";
}
oci_free_statement($s3);

// Clean up

$stmtarray = array(
    "drop procedure imp_res_get_2_proc",
    "drop table imp_res_get_2_tab_1",
    "drop table imp_res_get_2_tab_2"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECT--
Test 1
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
