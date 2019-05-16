--TEST--
Bug #51253 (oci_bind_array_by_name() array references)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

$drop = "DROP table bind_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

$create = "CREATE table bind_test(name VARCHAR(20))";
$statement = oci_parse($c, $create);
oci_execute($statement);

$create_pkg = "
CREATE OR REPLACE PACKAGE BUG51253_PKG AS
  TYPE ARRTYPE IS TABLE OF VARCHAR(20) INDEX BY BINARY_INTEGER;
  PROCEDURE iobind(c1 IN OUT ARRTYPE);
END BUG51253_PKG;";
$statement = oci_parse($c, $create_pkg);
oci_execute($statement);

$create_pkg_body = "
CREATE OR REPLACE PACKAGE BODY BUG51253_PKG AS
  CURSOR CUR IS SELECT name FROM bind_test;
  PROCEDURE iobind(c1 IN OUT ARRTYPE) IS
    BEGIN
    FOR i IN 1..5 LOOP
      INSERT INTO bind_test VALUES (c1(i));
    END LOOP;
    IF NOT CUR%ISOPEN THEN
      OPEN CUR;
    END IF;
    FOR i IN REVERSE 1..5 LOOP
      FETCH CUR INTO c1(i);
      IF CUR%NOTFOUND THEN
        CLOSE CUR;
        EXIT;
      END IF;
    END LOOP;
  END iobind;
END BUG51253_PKG;";
$statement = oci_parse($c, $create_pkg_body);
oci_execute($statement);

echo "Test 1\n";
$statement = oci_parse($c, "BEGIN bug51253_pkg.iobind(:c1); END;");
$array1 = array("one", "two", "three", "four", "five");
$array2 = $array1;
oci_bind_array_by_name($statement, ":c1", $array2, 5, -1, SQLT_CHR);
oci_execute($statement);

var_dump($array1);
var_dump($array2);


echo "Test 2\n";
$statement = oci_parse($c, "BEGIN bug51253_pkg.iobind(:c1); END;");
$array1 = array("one", "two", "three", "four", "five");
$array2 = &$array1;
oci_bind_array_by_name($statement, ":c1", $array2, 5, -1, SQLT_CHR);
oci_execute($statement);

var_dump($array1);
var_dump($array2);


echo "Test 3\n";
$statement = oci_parse($c, "BEGIN bug51253_pkg.iobind(:c1); END;");
$array1 = array("one", "two", "three", "four", "five");
$array2 = &$array1;
oci_bind_array_by_name($statement, ":c1", $array1, 5, -1, SQLT_CHR);
oci_execute($statement);

var_dump($array1);
var_dump($array2);

// Cleanup
$statement = oci_parse($c, "DROP PACKAGE BUG51253_PKG");
@oci_execute($statement);
$statement = oci_parse($c, "DROP TABLE BIND_TEST");
@oci_execute($statement);

echo "Done\n";
?>
--EXPECT--
Test 1
array(5) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(5) "three"
  [3]=>
  string(4) "four"
  [4]=>
  string(4) "five"
}
array(5) {
  [0]=>
  string(4) "five"
  [1]=>
  string(4) "four"
  [2]=>
  string(5) "three"
  [3]=>
  string(3) "two"
  [4]=>
  string(3) "one"
}
Test 2
array(5) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(5) "three"
  [3]=>
  string(4) "four"
  [4]=>
  string(4) "five"
}
array(5) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(5) "three"
  [3]=>
  string(4) "four"
  [4]=>
  string(4) "five"
}
Test 3
array(5) {
  [0]=>
  string(4) "five"
  [1]=>
  string(4) "four"
  [2]=>
  string(5) "three"
  [3]=>
  string(3) "two"
  [4]=>
  string(3) "one"
}
array(5) {
  [0]=>
  string(4) "five"
  [1]=>
  string(4) "four"
  [2]=>
  string(5) "three"
  [3]=>
  string(3) "two"
  [4]=>
  string(3) "one"
}
Done
