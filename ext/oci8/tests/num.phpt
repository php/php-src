--TEST--
oci_num_*() family
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table num_tab",
    "create table num_tab (id number, value number)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";
var_dump(ocirowcount());
var_dump(oci_num_rows());
var_dump(ocinumcols());
var_dump(oci_num_fields());

echo "Test 2\n";
$insert_sql = "insert into num_tab (id, value) values (1,1)";
if (!($s = oci_parse($c, $insert_sql))) {
    die("oci_parse(insert) failed!\n");
}

var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));

for ($i = 0; $i<3; $i++) {
  if (!oci_execute($s)) {
    die("oci_execute(insert) failed!\n");
  }
}

echo "Test 3\n";
var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));

if (!oci_commit($c)) {
  die("oci_commit() failed!\n");
}

echo "Test 4\n";
var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));

// All rows
$select_sql = "select * from num_tab";

if (!($s = oci_parse($c, $select_sql))) {
  die("oci_parse(select) failed!\n");
}

echo "Test 5a\n";
var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));

if (!oci_execute($s)) {
  die("oci_execute(select) failed!\n");
}

echo "Test 5b\n";
var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));


if (oci_fetch_all($s,$r) === false) {
  die("oci_fetch_all(select) failed!\n");
}

echo "Test 5c\n";
var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));

// One row
$select_sql = "SELECT id, value FROM num_tab WHERE ROWNUM < 2";

if (!($s = oci_parse($c, $select_sql))) {
  die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
  die("oci_execute(select) failed!\n");
}

if (oci_fetch_all($s,$r) === false) {
  die("oci_fetch_all(select) failed!\n");
}

echo "Test 6\n";
var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));

// No rows
$select_sql = "select id from num_tab where 1=0";

if (!($s = oci_parse($c, $select_sql))) {
  die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
  die("oci_execute(select) failed!\n");
}

if (oci_fetch_all($s,$r) === false) {
  die("oci_fetch_all(select) failed!\n");
}

echo "Test 7\n";
var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));

$delete_sql = "delete from num_tab";

if (!($s = oci_parse($c, $delete_sql))) {
    die("oci_parse(delete) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(delete) failed!\n");
}

echo "Test 8a\n";
var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));


oci_commit($c);

echo "Test 8b\n";
var_dump(ocirowcount($s));
var_dump(oci_num_rows($s));
var_dump(ocinumcols($s));
var_dump(oci_num_fields($s));


// Cleanup

$stmtarray = array(
    "drop table num_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECTF--
Test 1

Warning: ocirowcount() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_num_rows() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocinumcols() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_num_fields() expects exactly 1 parameter, 0 given in %s on line %d
NULL
Test 2
int(0)
int(0)
int(0)
int(0)
Test 3
int(1)
int(1)
int(0)
int(0)
Test 4
int(1)
int(1)
int(0)
int(0)
Test 5a
int(0)
int(0)
int(0)
int(0)
Test 5b
int(0)
int(0)
int(2)
int(2)
Test 5c
int(3)
int(3)
int(2)
int(2)
Test 6
int(1)
int(1)
int(2)
int(2)
Test 7
int(0)
int(0)
int(1)
int(1)
Test 8a
int(3)
int(3)
int(0)
int(0)
Test 8b
int(3)
int(3)
int(0)
int(0)
Done
