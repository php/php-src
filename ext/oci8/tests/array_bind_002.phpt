--TEST--
oci_bind_array_by_name() and invalid values 2
--EXTENSIONS--
oci8
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
CREATE OR REPLACE PACKAGE ARRAY_BIND_002_PKG AS
  TYPE ARRTYPE IS TABLE OF DATE INDEX BY BINARY_INTEGER;
  PROCEDURE iobind(c1 IN OUT ARRTYPE);
END ARRAY_BIND_002_PKG;";
$statement = oci_parse($c, $create_pkg);
oci_execute($statement);

$create_pkg_body = "
CREATE OR REPLACE PACKAGE BODY ARRAY_BIND_002_PKG AS
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
END ARRAY_BIND_002_PKG;";
$statement = oci_parse($c, $create_pkg_body);
oci_execute($statement);

$statement = oci_parse($c, "BEGIN array_bind_002_pkg.iobind(:c1); END;");

$array = Array("06-DEC-05","10-DEC-80","21-AUG-91","26-OCT-17","05-NOV-05");

try {
    oci_bind_array_by_name($statement, ":c1", $array, 0, 0, SQLT_ODT);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECTF--
oci_bind_array_by_name(): Argument #4 ($max_array_length) must be greater than 0

Warning: oci_execute(): ORA-%r(01008|57000)%r: %s in %s on line %d
array(5) {
  [0]=>
  string(9) "06-DEC-05"
  [1]=>
  string(9) "10-DEC-80"
  [2]=>
  string(9) "21-AUG-91"
  [3]=>
  string(9) "26-OCT-17"
  [4]=>
  string(9) "05-NOV-05"
}
Done
