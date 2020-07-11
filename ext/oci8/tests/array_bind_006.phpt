--TEST--
oci_bind_array_by_name(), SQLT_CHR and default max_length
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

$drop = "DROP table bind_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

$create = "CREATE table bind_test(name VARCHAR(20))";
$statement = oci_parse($c, $create);
oci_execute($statement);

$create_pkg = "
CREATE OR REPLACE PACKAGE ARRAY_BIND_006_PKG AS
  TYPE ARRTYPE IS TABLE OF VARCHAR(20) INDEX BY BINARY_INTEGER;
  PROCEDURE iobind(c1 IN OUT ARRTYPE);
END ARRAY_BIND_006_PKG;";
$statement = oci_parse($c, $create_pkg);
oci_execute($statement);

$create_pkg_body = "
CREATE OR REPLACE PACKAGE BODY ARRAY_BIND_006_PKG AS
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
END ARRAY_BIND_006_PKG;";
$statement = oci_parse($c, $create_pkg_body);
oci_execute($statement);

$statement = oci_parse($c, "BEGIN array_bind_006_pkg.iobind(:c1); END;");

$array = array("one", "two", "three", "four", "five");

oci_bind_array_by_name($statement, ":c1", $array, 5, -1, SQLT_CHR);

oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECT--
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
