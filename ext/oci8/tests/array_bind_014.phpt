--TEST--
oci_bind_array_by_name() and NUMBERs
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

$drop = "DROP table bind_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

$create = "CREATE table bind_test(name NUMBER)";
$statement = oci_parse($c, $create);
oci_execute($statement);

$create_pkg = "
CREATE OR REPLACE PACKAGE ARRAY_BIND_014_PKG AS
  TYPE ARRTYPE IS TABLE OF NUMBER INDEX BY BINARY_INTEGER;
  PROCEDURE iobind(c1 IN OUT ARRTYPE);
END ARRAY_BIND_014_PKG;";
$statement = oci_parse($c, $create_pkg);
oci_execute($statement);

$create_pkg_body = "
CREATE OR REPLACE PACKAGE BODY ARRAY_BIND_014_PKG AS
  CURSOR CUR IS SELECT name FROM bind_test;
  PROCEDURE iobind(c1 IN OUT ARRTYPE) IS
    BEGIN
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
END ARRAY_BIND_014_PKG;";
$statement = oci_parse($c, $create_pkg_body);
oci_execute($statement);

for ($i = 1; $i < 6; $i++) {
    $statement = oci_parse($c, "INSERT INTO bind_test VALUES (".$i.")");
    oci_execute($statement);
}

$statement = oci_parse($c, "BEGIN array_bind_014_pkg.iobind(:c1); END;");
$array = Array();
oci_bind_array_by_name($statement, ":c1", $array, 5, -1, SQLT_INT);
oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECT--
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(4)
  [2]=>
  int(3)
  [3]=>
  int(2)
  [4]=>
  int(1)
}
Done
