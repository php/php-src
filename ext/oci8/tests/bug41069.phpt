--TEST--
Bug #41069 (Oracle crash with certain data over a DB-link when prefetch memory limit used - Oracle bug 6039623)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
if (empty($dbase)) die ("skip requires network connection alias for DB link loopback");
if ($test_drcp) die("skip DRCP does not support shared database links");
?>
--INI--
oci8.default_prefetch=5
--FILE--
    <?php

    require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "alter session set nls_date_format = 'MM/DD/YYYY'",

    "drop database link bug41069_dblink",

    "drop table bug41069_tab",

    "create shared database link bug41069_dblink authenticated by $user identified by $password using '$dbase'",

    "create table bug41069_tab
    (
        c1  number(20),
        c2  varchar2(60 byte),
        c3  varchar2(1000 byte),
        c4  varchar2(255 byte),
        c5  varchar2(2 byte),
        c6  varchar2(1 byte),
        c7  varchar2(255 byte),
        c8  varchar2(50 byte),
        c9  date,
        c10 date,
        c12 number(20),
        c13 varchar2(20 byte),
        c15 varchar2(50 byte)
     )",

    "insert into bug41069_tab (c1, c2, c5, c6, c9, c10, c12, c15)	values
    (111, 'aaaaaaa', 'b', 'c', '01/17/2008', '01/07/2017', 2222, 'zzzzzzzzzz')",

    "insert into bug41069_tab (c1, c2, c3, c4, c5, c6, c7, c9, c10, c12, c13, c15) values
    (112, 'aaaaaaa', 'bbbbbbbb', 'ccccccc', 'd', 'e', 'rrrrrrr', '04/16/2007', '04/16/2007', 2223, 'xxxxxxxx', 'zzzzzzzz')",

    "insert into bug41069_tab (c1, c2, c3, c4, c5, c6, c7, c9, c10, c12, c15)	values
    (113, 'aaaaaaa', 'bbbbbbbbbb', 'cccccc', 'e', 'f', 'dddd', '12/04/2006', '12/04/2006', 2224, 'zzzzzzz')"
);

oci8_test_sql_execute($c, $stmtarray);


// Run Tests

echo "Test 1: non-DB link case that always worked\n";
$stid = oci_parse($c, 'select * from bug41069_tab order by c1');
oci_execute($stid, OCI_DEFAULT);
oci_fetch_all($stid, $results, 0, -1, OCI_ASSOC+OCI_FETCHSTATEMENT_BY_ROW);
var_dump($results);

echo "Test 2: Should not crash\n";
$stid = oci_parse($c, 'select * from bug41069_tab@bug41069_dblink order by c1');
oci_execute($stid, OCI_DEFAULT);
oci_fetch_all($stid, $results, 0, -1, OCI_ASSOC+OCI_FETCHSTATEMENT_BY_ROW);
var_dump($results);

// Cleanup

$c = oci_new_connect($user, $password, $dbase);

$stmtarray = array(
    "drop database link bug41069_dblink",
    "drop table bug41069_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
Test 1: non-DB link case that always worked
array(3) {
  [0]=>
  array(13) {
    ["C1"]=>
    string(3) "111"
    ["C2"]=>
    string(7) "aaaaaaa"
    ["C3"]=>
    NULL
    ["C4"]=>
    NULL
    ["C5"]=>
    string(1) "b"
    ["C6"]=>
    string(1) "c"
    ["C7"]=>
    NULL
    ["C8"]=>
    NULL
    ["C9"]=>
    string(10) "01/17/2008"
    ["C10"]=>
    string(10) "01/07/2017"
    ["C12"]=>
    string(4) "2222"
    ["C13"]=>
    NULL
    ["C15"]=>
    string(10) "zzzzzzzzzz"
  }
  [1]=>
  array(13) {
    ["C1"]=>
    string(3) "112"
    ["C2"]=>
    string(7) "aaaaaaa"
    ["C3"]=>
    string(8) "bbbbbbbb"
    ["C4"]=>
    string(7) "ccccccc"
    ["C5"]=>
    string(1) "d"
    ["C6"]=>
    string(1) "e"
    ["C7"]=>
    string(7) "rrrrrrr"
    ["C8"]=>
    NULL
    ["C9"]=>
    string(10) "04/16/2007"
    ["C10"]=>
    string(10) "04/16/2007"
    ["C12"]=>
    string(4) "2223"
    ["C13"]=>
    string(8) "xxxxxxxx"
    ["C15"]=>
    string(8) "zzzzzzzz"
  }
  [2]=>
  array(13) {
    ["C1"]=>
    string(3) "113"
    ["C2"]=>
    string(7) "aaaaaaa"
    ["C3"]=>
    string(10) "bbbbbbbbbb"
    ["C4"]=>
    string(6) "cccccc"
    ["C5"]=>
    string(1) "e"
    ["C6"]=>
    string(1) "f"
    ["C7"]=>
    string(4) "dddd"
    ["C8"]=>
    NULL
    ["C9"]=>
    string(10) "12/04/2006"
    ["C10"]=>
    string(10) "12/04/2006"
    ["C12"]=>
    string(4) "2224"
    ["C13"]=>
    NULL
    ["C15"]=>
    string(7) "zzzzzzz"
  }
}
Test 2: Should not crash
array(3) {
  [0]=>
  array(13) {
    ["C1"]=>
    string(3) "111"
    ["C2"]=>
    string(7) "aaaaaaa"
    ["C3"]=>
    NULL
    ["C4"]=>
    NULL
    ["C5"]=>
    string(1) "b"
    ["C6"]=>
    string(1) "c"
    ["C7"]=>
    NULL
    ["C8"]=>
    NULL
    ["C9"]=>
    string(10) "01/17/2008"
    ["C10"]=>
    string(10) "01/07/2017"
    ["C12"]=>
    string(4) "2222"
    ["C13"]=>
    NULL
    ["C15"]=>
    string(10) "zzzzzzzzzz"
  }
  [1]=>
  array(13) {
    ["C1"]=>
    string(3) "112"
    ["C2"]=>
    string(7) "aaaaaaa"
    ["C3"]=>
    string(8) "bbbbbbbb"
    ["C4"]=>
    string(7) "ccccccc"
    ["C5"]=>
    string(1) "d"
    ["C6"]=>
    string(1) "e"
    ["C7"]=>
    string(7) "rrrrrrr"
    ["C8"]=>
    NULL
    ["C9"]=>
    string(10) "04/16/2007"
    ["C10"]=>
    string(10) "04/16/2007"
    ["C12"]=>
    string(4) "2223"
    ["C13"]=>
    string(8) "xxxxxxxx"
    ["C15"]=>
    string(8) "zzzzzzzz"
  }
  [2]=>
  array(13) {
    ["C1"]=>
    string(3) "113"
    ["C2"]=>
    string(7) "aaaaaaa"
    ["C3"]=>
    string(10) "bbbbbbbbbb"
    ["C4"]=>
    string(6) "cccccc"
    ["C5"]=>
    string(1) "e"
    ["C6"]=>
    string(1) "f"
    ["C7"]=>
    string(4) "dddd"
    ["C8"]=>
    NULL
    ["C9"]=>
    string(10) "12/04/2006"
    ["C10"]=>
    string(10) "12/04/2006"
    ["C12"]=>
    string(4) "2224"
    ["C13"]=>
    NULL
    ["C15"]=>
    string(7) "zzzzzzz"
  }
}
Done
