--TEST--
oci_field_*() family: basic column types
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

// Initialization
$stmtarray = array(
    "drop table field_funcs3_tab",
    "create table field_funcs3_tab(c1_c char(2), c2_v varchar2(2), c3_n number, c4_d date)",
    "insert into field_funcs3_tab values ('c1', 'c2', 3, '01-Jan-2010')"
);

$v = oci_server_version($c);
if (strpos($v, 'Oracle TimesTen') === false) {
    oci8_test_sql_execute($c, array("alter session set nls_date_format = 'DD-MON-YYYY'"));
}
oci8_test_sql_execute($c, $stmtarray);

// Run Test

$select_sql = "select * from field_funcs3_tab";

if (!($s = oci_parse($c, $select_sql))) {
    die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}

$row = oci_fetch_array($s, OCI_NUM + OCI_RETURN_NULLS + OCI_RETURN_LOBS);
var_dump($row);

foreach ($row as $num => $field) {
    $num++;
    var_dump(oci_field_is_null($s, $num));
    var_dump(oci_field_name($s, $num));
    var_dump(oci_field_type($s, $num));
    var_dump(oci_field_type_raw($s, $num));
    var_dump(oci_field_scale($s, $num));
    var_dump(oci_field_precision($s, $num));
    var_dump(oci_field_size($s, $num));
}

// Clean up

$stmtarray = array(
    "drop table field_funcs3_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
array(4) {
  [0]=>
  string(2) "c1"
  [1]=>
  string(2) "c2"
  [2]=>
  string(1) "3"
  [3]=>
  string(1%r[19]%r) "%r(01-JAN-2010|0001-01-20 10:00:00)%r"
}
bool(false)
string(4) "C1_C"
string(4) "CHAR"
int(96)
int(0)
int(0)
int(2)
bool(false)
string(4) "C2_V"
string(8) "VARCHAR2"
int(1)
int(0)
int(0)
int(2)
bool(false)
string(4) "C3_N"
string(6) "NUMBER"
int(2)
int(-127)
int(0)
int(22)
bool(false)
string(4) "C4_D"
string(4) "DATE"
int(12)
int(0)
int(0)
int(7)
