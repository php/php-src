--TEST--
Bug #74625 (Integer overflow in oci_bind_array_by_name)
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
?>
--FILE--
<?php
require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
        "CREATE TABLE bug74625_tab (NAME NUMBER)",
        "CREATE OR REPLACE PACKAGE PKG74625 AS
          TYPE ARRTYPE IS TABLE OF NUMBER INDEX BY BINARY_INTEGER;
          PROCEDURE iobind(c1 IN OUT ARRTYPE);
         END PKG74625;",
        "CREATE OR REPLACE PACKAGE BODY PKG74625 AS
          PROCEDURE iobind(c1 IN OUT ARRTYPE) IS
          BEGIN
           FOR i IN 1..5 LOOP
            c1(i) := c1(i) * 2;
           END LOOP;
          END iobind;
         END PKG74625;"
);

oci8_test_sql_execute($c, $stmtarray);

$statement = oci_parse($c, "BEGIN pkg74625.iobind(:c1); END;");

$array = Array(-1,-2,-3,-4,-5);

oci_bind_array_by_name($statement, ":c1", $array, 5, -1, SQLT_INT);

oci_execute($statement);

var_dump($array);

// Cleanup
$stmtarray = array(
    "DROP TABLE bug74625_tab",
    "DROP PACKAGE PKG74625"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
array(5) {
  [0]=>
  int(-2)
  [1]=>
  int(-4)
  [2]=>
  int(-6)
  [3]=>
  int(-8)
  [4]=>
  int(-10)
}
===DONE===
