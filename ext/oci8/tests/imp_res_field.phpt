--TEST--
Oracle Database 12c Implicit Result Sets: field tests
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

$stmtarray = array(
    "drop table imp_res_field_tab_1",
    "create table imp_res_field_tab_1 (c1_number number, c2_varchar210 varchar2(10))",
    "insert into imp_res_field_tab_1 values (1111, 'abcde')",

    "drop table imp_res_field_tab_2",
    "create table imp_res_field_tab_2 (c3_varchar21 varchar2(4))",
    "insert into imp_res_field_tab_2 values ('tttt')",

    "drop table imp_res_field_tab_3",
    "create table imp_res_field_tab_3 (c4_number52 number(5,2))",
    "insert into imp_res_field_tab_3 values (33)",
    "insert into imp_res_field_tab_3 values (NULL)",

    "create or replace procedure imp_res_field_proc as
      c1 sys_refcursor;
    begin
      open c1 for select * from imp_res_field_tab_1 order by 1;
      dbms_sql.return_result(c1);

      open c1 for select * from imp_res_field_tab_2 order by 1;
      dbms_sql.return_result(c1);

      open c1 for select * from imp_res_field_tab_3 order by 1;
      dbms_sql.return_result(c1);
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

function print_fields($s)
{
    echo "num fields : " . oci_num_fields($s) . "\n";
    for ($i = 1; $i <= oci_num_fields($s); $i++) {
        $is_null = oci_field_is_null($s, $i) ? "T" : "F";
        $name = oci_field_name($s, $i);
        $precision = oci_field_precision($s, $i);
        $scale = oci_field_scale($s, $i);
        $size = oci_field_size($s, $i);
        $typeraw = oci_field_type_raw($s, $i);
        $type = oci_field_type($s, $i);
        echo "$name\t: is_null $is_null, precision $precision, scale $scale, size $size, typeraw $typeraw, type $type\n";
    }
}

// Run Test

echo "Test 1 - can't get IRS fields from parent\n";
$s = oci_parse($c, "begin imp_res_field_proc(); end;");
oci_execute($s);
print_fields($s);

echo "\nTest 2 - can't get IRS fields from parent when fetching\n";
$s = oci_parse($c, "begin imp_res_field_proc(); end;");
oci_execute($s);
while (($r = oci_fetch_row($s))) {
    var_dump($r);
    print_fields($s);
}

echo "\nTest 3 - get IRS fields\n";
$s = oci_parse($c, "begin imp_res_field_proc(); end;");
oci_execute($s);
while (($s1 = oci_get_implicit_resultset($s))) {
    print_fields($s1);
}

echo "\nTest 4 - get IRS fields before fetching rows\n";
$s = oci_parse($c, "begin imp_res_field_proc(); end;");
oci_execute($s);
$i = 0;
while (($s1 = oci_get_implicit_resultset($s))) {
    echo "===> Result set ".++$i."\n";
    print_fields($s1);
    while (($r = oci_fetch_row($s1)) !== false) {
        var_dump($r);
    }
}

echo "\nTest 5 - get IRS fields when fetching rows\n";
$s = oci_parse($c, "begin imp_res_field_proc(); end;");
oci_execute($s);
$i = 0;
while (($s1 = oci_get_implicit_resultset($s))) {
    echo "===> Result set ".++$i."\n";
    while (($r = oci_fetch_row($s1)) !== false) {
        var_dump($r);
        print_fields($s1);
    }
}

// Clean up

$stmtarray = array(
    "drop procedure imp_res_field_proc",
    "drop table imp_res_field_tab_1",
    "drop table imp_res_field_tab_2",
    "drop table imp_res_field_tab_3"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1 - can't get IRS fields from parent
num fields : 0

Test 2 - can't get IRS fields from parent when fetching
array(2) {
  [0]=>
  string(4) "1111"
  [1]=>
  string(5) "abcde"
}
num fields : 0
array(1) {
  [0]=>
  string(4) "tttt"
}
num fields : 0
array(1) {
  [0]=>
  string(2) "33"
}
num fields : 0
array(1) {
  [0]=>
  NULL
}
num fields : 0

Test 3 - get IRS fields
num fields : 2
C1_NUMBER	: is_null F, precision 0, scale -127, size 22, typeraw 2, type NUMBER
C2_VARCHAR210	: is_null F, precision 0, scale 0, size 10, typeraw 1, type VARCHAR2
num fields : 1
C3_VARCHAR21	: is_null F, precision 0, scale 0, size 4, typeraw 1, type VARCHAR2
num fields : 1
C4_NUMBER52	: is_null F, precision 5, scale 2, size 22, typeraw 2, type NUMBER

Test 4 - get IRS fields before fetching rows
===> Result set 1
num fields : 2
C1_NUMBER	: is_null F, precision 0, scale -127, size 22, typeraw 2, type NUMBER
C2_VARCHAR210	: is_null F, precision 0, scale 0, size 10, typeraw 1, type VARCHAR2
array(2) {
  [0]=>
  string(4) "1111"
  [1]=>
  string(5) "abcde"
}
===> Result set 2
num fields : 1
C3_VARCHAR21	: is_null F, precision 0, scale 0, size 4, typeraw 1, type VARCHAR2
array(1) {
  [0]=>
  string(4) "tttt"
}
===> Result set 3
num fields : 1
C4_NUMBER52	: is_null F, precision 5, scale 2, size 22, typeraw 2, type NUMBER
array(1) {
  [0]=>
  string(2) "33"
}
array(1) {
  [0]=>
  NULL
}

Test 5 - get IRS fields when fetching rows
===> Result set 1
array(2) {
  [0]=>
  string(4) "1111"
  [1]=>
  string(5) "abcde"
}
num fields : 2
C1_NUMBER	: is_null F, precision 0, scale -127, size 22, typeraw 2, type NUMBER
C2_VARCHAR210	: is_null F, precision 0, scale 0, size 10, typeraw 1, type VARCHAR2
===> Result set 2
array(1) {
  [0]=>
  string(4) "tttt"
}
num fields : 1
C3_VARCHAR21	: is_null F, precision 0, scale 0, size 4, typeraw 1, type VARCHAR2
===> Result set 3
array(1) {
  [0]=>
  string(2) "33"
}
num fields : 1
C4_NUMBER52	: is_null F, precision 5, scale 2, size 22, typeraw 2, type NUMBER
array(1) {
  [0]=>
  NULL
}
num fields : 1
C4_NUMBER52	: is_null T, precision 5, scale 2, size 22, typeraw 2, type NUMBER
===DONE===
