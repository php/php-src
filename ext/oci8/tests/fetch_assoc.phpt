--TEST--
oci_fetch_assoc()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require __DIR__."/connect.inc";

$stmtarray = array(
    "drop table fetch_assoc_tab",
    "create table fetch_assoc_tab (id number, value number, dummy varchar2(20))",
    "insert into fetch_assoc_tab values (1,1,null)",
    "insert into fetch_assoc_tab values (1,1,null)",
    "insert into fetch_assoc_tab values (1,1,null)"
);

oci8_test_sql_execute($c, $stmtarray);

$select_sql = "select * from fetch_assoc_tab";

if (!($s = oci_parse($c, $select_sql))) {
    die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
    die("oci_execute(select) failed!\n");
}
while ($row = oci_fetch_assoc($s)) {
    var_dump($row);
}

// Clean up

$stmtarray = array(
    "drop table fetch_assoc_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
array(3) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
  ["DUMMY"]=>
  NULL
}
array(3) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
  ["DUMMY"]=>
  NULL
}
array(3) {
  ["ID"]=>
  string(1) "1"
  ["VALUE"]=>
  string(1) "1"
  ["DUMMY"]=>
  NULL
}
Done
