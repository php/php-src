--TEST--
Bug #40078 (ORA-01405 when fetching NULL values using oci_bind_array_by_name())
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

$create_pkg = "
CREATE OR REPLACE PACKAGE BUG40078_PKG AS
    TYPE ARRTYPE IS TABLE OF VARCHAR(20) INDEX BY BINARY_INTEGER;
    PROCEDURE nullbind(c1 OUT ARRTYPE);
END BUG40078_PKG;";
$statement = oci_parse($c, $create_pkg);
oci_execute($statement);

$create_pkg_body = "
CREATE OR REPLACE PACKAGE BODY BUG40078_PKG AS
    PROCEDURE nullbind(c1 OUT ARRTYPE) IS
    BEGIN
        c1(1) := 'one';
        c1(2) := 'two';
        c1(3) := '';
        c1(4) := 'four';
        c1(5) := 'five';
    END nullbind;
END BUG40078_PKG;";
$statement = oci_parse($c, $create_pkg_body);
oci_execute($statement);

$statement = oci_parse($c, "BEGIN bug40078_pkg.nullbind(:c1); END;");

oci_bind_array_by_name($statement, ":c1", $array, 5, 20, SQLT_CHR);

oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECT--
array(5) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(0) ""
  [3]=>
  string(4) "four"
  [4]=>
  string(4) "five"
}
Done
