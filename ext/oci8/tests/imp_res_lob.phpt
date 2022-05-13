--TEST--
Oracle Database 12c Implicit Result Sets: LOBs
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
    "drop table imp_res_lob_tab",
    "create table imp_res_lob_tab (c1 number, c2 clob, c3 varchar2(10))",
    "insert into imp_res_lob_tab values (1, 'aaaaa', 'a')",
    "insert into imp_res_lob_tab values (2, 'bbbbb', 'b')",
    "insert into imp_res_lob_tab values (3, 'ccccc', 'c')",
    "insert into imp_res_lob_tab values (4, 'ddddd', 'd')",

    "create or replace procedure imp_res_lob_proc as
      c1 sys_refcursor;
    begin
      open c1 for select * from imp_res_lob_tab order by 1;
      dbms_sql.return_result(c1);
      open c1 for select * from dual;
      dbms_sql.return_result(c1);
      open c1 for select c2 from imp_res_lob_tab order by c1;
      dbms_sql.return_result(c1);
   end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";
$s = oci_parse($c, "begin imp_res_lob_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_row($s)) != false) {
    foreach ($row as $item) {
        if (is_object($item)) {
            echo " " . $item->load();
        } else {
            echo " " . $item;
        }
    }
    echo "\n";
}

echo "\nTest 2 - don't fetch all rows\n";
$s = oci_parse($c, "begin imp_res_lob_proc(); end;");
oci_execute($s);
$row = oci_fetch_row($s);
foreach ($row as $item) {
    if (is_object($item)) {
        echo " " . $item->load();
    } else {
        echo " " . $item;
    }
}
echo "\n";

// Clean up

$stmtarray = array(
    "drop procedure imp_res_lob_proc",
    "drop table imp_res_lob_tab",
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECT--
Test 1
 1 aaaaa a
 2 bbbbb b
 3 ccccc c
 4 ddddd d
 X
 aaaaa
 bbbbb
 ccccc
 ddddd

Test 2 - don't fetch all rows
 1 aaaaa a
