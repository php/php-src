--TEST--
SELECT tests with duplicate column anmes
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table dupcolnames_tab1",
    "drop table dupcolnames_tab2",

    "create table dupcolnames_tab1 (c1 number, dupnamecol varchar2(20))",
    "create table dupcolnames_tab2 (c2 number, dupnamecol varchar2(20))",

    "insert into dupcolnames_tab1 (c1, dupnamecol) values (1, 'chris')",
    "insert into dupcolnames_tab2 (c2, dupnamecol) values (2, 'jones')",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1 - OCI_ASSOC\n";
$s = oci_parse($c, "select * from dupcolnames_tab1, dupcolnames_tab2");
oci_execute($s);
while (($r = oci_fetch_array($s, OCI_ASSOC)) != false) {
    var_dump($r);
}


echo "\nTest 2 - OCI_NUM\n";
$s = oci_parse($c, "select * from dupcolnames_tab1, dupcolnames_tab2");
oci_execute($s);
while (($r = oci_fetch_array($s, OCI_NUM)) != false) {
    var_dump($r);
}


echo "\nTest 3 - OCI_ASSOC+OCI_NUM\n";
$s = oci_parse($c, "select * from dupcolnames_tab1, dupcolnames_tab2");
oci_execute($s);
while (($r = oci_fetch_array($s, OCI_ASSOC+OCI_NUM)) != false) {
    var_dump($r);
}

// Clean up

$stmtarray = array(
    "drop table dupcolnames_tab1",
    "drop table dupcolnames_tab2",
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1 - OCI_ASSOC
array(3) {
  ["C1"]=>
  string(1) "1"
  ["DUPNAMECOL"]=>
  string(5) "jones"
  ["C2"]=>
  string(1) "2"
}

Test 2 - OCI_NUM
array(4) {
  [0]=>
  string(1) "1"
  [1]=>
  string(5) "chris"
  [2]=>
  string(1) "2"
  [3]=>
  string(5) "jones"
}

Test 3 - OCI_ASSOC+OCI_NUM
array(7) {
  [0]=>
  string(1) "1"
  ["C1"]=>
  string(1) "1"
  [1]=>
  string(5) "chris"
  ["DUPNAMECOL"]=>
  string(5) "jones"
  [2]=>
  string(1) "2"
  ["C2"]=>
  string(1) "2"
  [3]=>
  string(5) "jones"
}
===DONE===
