--TEST--
Bind tests with SQLT_AFC
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
    "drop table bind_sqltafc_tab",
    "create table bind_sqltafc_tab (id number, char_t char(1), char_t10 char(10), varchar2_t10 varchar2(10), number_t number)",
    "insert into bind_sqltafc_tab values (0, 'a', 'abcd', 'efghij', 1.1)"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

function q($c, $id)
{
    $s = oci_parse($c, "select * from bind_sqltafc_tab where id = $id");
    oci_execute($s);
    oci_fetch_all($s, $r);
    var_dump($r);
}

echo "Test 0 - base table creation without binds\n";

q($c, 0);

echo "\nTest 1 - successful insert\n";

$s = oci_parse($c, "INSERT INTO bind_sqltafc_tab (id, char_t, char_t10, varchar2_t10, number_t) VALUES (1, :c2, :c3, :c4, :c5)");
$c2 = "H";
$c3 = "AAAAAAAAAA";  // max length allowed in column
$c4 = "BBBBBBBBBB";  // max length allowed in column
$c5 = "123.45";
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_AFC);
oci_bind_by_name($s, ":c3", $c3, -1, SQLT_AFC);
oci_bind_by_name($s, ":c4", $c4, -1, SQLT_AFC);
oci_bind_by_name($s, ":c5", $c5, -1, SQLT_AFC);
oci_execute($s);

q($c, 1);

echo "\nTest 2 - Empty Strings\n";

$s = oci_parse($c, "INSERT INTO bind_sqltafc_tab (id, char_t, char_t10, varchar2_t10, number_t) VALUES (5, :c2, :c3, :c4, :c5)");
$c2 = "";
$c3 = "";
$c4 = "";
$c5 = "";
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_AFC);
oci_bind_by_name($s, ":c3", $c3, -1, SQLT_AFC);
oci_bind_by_name($s, ":c4", $c4, -1, SQLT_AFC);
oci_bind_by_name($s, ":c5", $c5, -1, SQLT_AFC);
oci_execute($s);

q($c, 5);

echo "\nTest 3 - NULLs\n";

$s = oci_parse($c, "INSERT INTO bind_sqltafc_tab (id, char_t, char_t10, varchar2_t10, number_t) VALUES (6, :c2, :c3, :c4, :c5)");
$c2 = null;
$c3 = null;
$c4 = null;
$c5 = null;
oci_bind_by_name($s, ":c2", $c2, -1, SQLT_AFC);
oci_bind_by_name($s, ":c3", $c3, -1, SQLT_AFC);
oci_bind_by_name($s, ":c4", $c4, -1, SQLT_AFC);
oci_bind_by_name($s, ":c5", $c5, -1, SQLT_AFC);
oci_execute($s);

q($c, 6);

// Clean up

$stmtarray = array(
    "drop table bind_sqltafc_tab"
);

oci8_test_sql_execute($c, $stmtarray);

oci_close($c);

?>
--EXPECT--
Test 0 - base table creation without binds
array(5) {
  ["ID"]=>
  array(1) {
    [0]=>
    string(1) "0"
  }
  ["CHAR_T"]=>
  array(1) {
    [0]=>
    string(1) "a"
  }
  ["CHAR_T10"]=>
  array(1) {
    [0]=>
    string(10) "abcd      "
  }
  ["VARCHAR2_T10"]=>
  array(1) {
    [0]=>
    string(6) "efghij"
  }
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(3) "1.1"
  }
}

Test 1 - successful insert
array(5) {
  ["ID"]=>
  array(1) {
    [0]=>
    string(1) "1"
  }
  ["CHAR_T"]=>
  array(1) {
    [0]=>
    string(1) "H"
  }
  ["CHAR_T10"]=>
  array(1) {
    [0]=>
    string(10) "AAAAAAAAAA"
  }
  ["VARCHAR2_T10"]=>
  array(1) {
    [0]=>
    string(10) "BBBBBBBBBB"
  }
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    string(6) "123.45"
  }
}

Test 2 - Empty Strings
array(5) {
  ["ID"]=>
  array(1) {
    [0]=>
    string(1) "5"
  }
  ["CHAR_T"]=>
  array(1) {
    [0]=>
    NULL
  }
  ["CHAR_T10"]=>
  array(1) {
    [0]=>
    NULL
  }
  ["VARCHAR2_T10"]=>
  array(1) {
    [0]=>
    NULL
  }
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    NULL
  }
}

Test 3 - NULLs
array(5) {
  ["ID"]=>
  array(1) {
    [0]=>
    string(1) "6"
  }
  ["CHAR_T"]=>
  array(1) {
    [0]=>
    NULL
  }
  ["CHAR_T10"]=>
  array(1) {
    [0]=>
    NULL
  }
  ["VARCHAR2_T10"]=>
  array(1) {
    [0]=>
    NULL
  }
  ["NUMBER_T"]=>
  array(1) {
    [0]=>
    NULL
  }
}
