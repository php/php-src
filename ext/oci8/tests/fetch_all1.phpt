--TEST--
oci_fetch_all()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table fetch_all_tab",
    "create table fetch_all_tab (id number, value number)",
    "insert into fetch_all_tab (id, value) values (1,1)",
    "insert into fetch_all_tab (id, value) values (1,1)",
    "insert into fetch_all_tab (id, value) values (1,1)"
);

oci8_test_sql_execute($c, $stmtarray);

if (!($s = oci_parse($c, "select * from fetch_all_tab"))) {
    die("oci_parse(select) failed!\n");
}

/* oci_fetch_all */
if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}
var_dump(oci_fetch_all($s, $all));
var_dump($all);

// Cleanup

$stmtarray = array(
    "drop table fetch_all_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
int(3)
array(2) {
  ["ID"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    [2]=>
    string(1) "1"
  }
  ["VALUE"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "1"
    [2]=>
    string(1) "1"
  }
}
Done
