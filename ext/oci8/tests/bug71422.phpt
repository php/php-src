--TEST--
Bug #71422 (Fix ORA-01438: value larger than specified precision allowed for this column)
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

$stmtarray = array(
	"DROP TABLE BUG71422_TEST",
	"CREATE TABLE BUG71422_TEST (TEST_ID NUMBER(*,0) NOT NULL, LABEL VARCHAR2(50 CHAR), CONSTRAINT BUG71422_TEST_PK PRIMARY KEY (TEST_ID))",
	"INSERT INTO BUG71422_TEST (TEST_ID, LABEL) VALUES (1, 'Foo')"
);

oci8_test_sql_execute($c, $stmtarray);

$stmt = oci_parse($c, 'SELECT LABEL AS RAW_QUERY FROM BUG71422_TEST WHERE TEST_ID=1');
oci_execute($stmt);
while ($row = oci_fetch_array($stmt, OCI_ASSOC+OCI_RETURN_NULLS)) {
	var_dump($row);
}

$stmt = oci_parse($c, 'SELECT LABEL AS NUMERIC_BIND_PARAMETER FROM BUG71422_TEST WHERE TEST_ID=:test_id');
$value = 1;
oci_bind_by_name($stmt, ':test_id', $value, -1, SQLT_INT);
oci_execute($stmt);
while ($row = oci_fetch_array($stmt, OCI_ASSOC+OCI_RETURN_NULLS)) {
	var_dump($row);
}

$stmt = oci_parse($c, 'SELECT LABEL AS STRING_BIND_PARAMETER FROM BUG71422_TEST WHERE TEST_ID=:test_id');
$value = 1;
oci_bind_by_name($stmt, ':test_id', $value, -1, SQLT_CHR);
oci_execute($stmt);
while ($row = oci_fetch_array($stmt, OCI_ASSOC+OCI_RETURN_NULLS)) {
	var_dump($row);
}

// Cleanup

$stmtarray = array(
	"DROP TABLE BUG71422_TEST"
);
oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
array(1) {
  ["RAW_QUERY"]=>
  string(3) "Foo"
}
array(1) {
  ["NUMERIC_BIND_PARAMETER"]=>
  string(3) "Foo"
}
array(1) {
  ["STRING_BIND_PARAMETER"]=>
  string(3) "Foo"
}
===DONE===
