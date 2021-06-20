--TEST--
oci_bind_array_by_name() and invalid values 3
--EXTENSIONS--
oci8
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

$create = "CREATE table bind_test(name DATE)";
$statement = oci_parse($c, $create);
oci_execute($statement);

$create_pkg = "
CREATE OR REPLACE PACKAGE ARRAY_BIND_003_PKG AS
  TYPE ARRTYPE IS TABLE OF DATE INDEX BY BINARY_INTEGER;
  PROCEDURE iobind(c1 IN OUT ARRTYPE);
END ARRAY_BIND_003_PKG;";
$statement = oci_parse($c, $create_pkg);
oci_execute($statement);

$create_pkg_body = "
CREATE OR REPLACE PACKAGE BODY ARRAY_BIND_003_PKG AS
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
END ARRAY_BIND_003_PKG;";
$statement = oci_parse($c, $create_pkg_body);
oci_execute($statement);

$statement = oci_parse($c, "BEGIN array_bind_003_pkg.iobind(:c1); END;");

$array = Array("06-DEC-05","10-DEC-80","21-AUG-91","26-OCT-17");

oci_bind_array_by_name($statement, ":c1", $array, 5, 5, SQLT_ODT);

oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECTF--
Warning: oci_execute(): ORA-01403: %s
ORA-06512: at "%s.ARRAY_BIND_003_PKG", line %d
ORA-06512: at line %d in %sarray_bind_003.php on line %d
array(4) {
  [0]=>
  string(9) "06-DEC-05"
  [1]=>
  string(9) "10-DEC-80"
  [2]=>
  string(9) "21-AUG-91"
  [3]=>
  string(9) "26-OCT-17"
}
Done
