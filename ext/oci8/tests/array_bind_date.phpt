--TEST--
oci_bind_array_by_name() and SQLT_ODT
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

$drop = "DROP table bind_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

$create = "CREATE table bind_test(name DATE)";
$statement = oci_parse($c, $create);
oci_execute($statement);

$create_pkg = "
CREATE OR REPLACE PACKAGE ARRAY_BIND_DATE_PKG AS
  TYPE ARRTYPE IS TABLE OF DATE INDEX BY BINARY_INTEGER;
  PROCEDURE iobind(c1 IN OUT ARRTYPE);
END ARRAY_BIND_DATE_PKG;";
$statement = oci_parse($c, $create_pkg);
oci_execute($statement);

$create_pkg_body = "
CREATE OR REPLACE PACKAGE BODY ARRAY_BIND_DATE_PKG AS
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
END ARRAY_BIND_DATE_PKG;";
$statement = oci_parse($c, $create_pkg_body);
oci_execute($statement);

$statement = oci_parse($c, "BEGIN array_bind_date_pkg.iobind(:c1); END;");

$array = Array("06-DEC-05","10-DEC-80","21-AUG-91","26-OCT-17","05-NOV-05");

oci_bind_array_by_name($statement, ":c1", $array, 5, 5, SQLT_ODT);

oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECT--
array(5) {
  [0]=>
  string(9) "05-NOV-05"
  [1]=>
  string(9) "26-OCT-17"
  [2]=>
  string(9) "21-AUG-91"
  [3]=>
  string(9) "10-DEC-80"
  [4]=>
  string(9) "06-DEC-05"
}
Done
