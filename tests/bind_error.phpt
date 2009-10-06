--TEST--
Test some oci_bind_by_name error conditions
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

$drop = "drop table bind_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

$create = "create table bind_test(name varchar(10))";
$statement = oci_parse($c, $create);
oci_execute($statement);


echo "Insert value\n";

$name = 'abc';
$stmt = oci_parse($c, "insert into bind_test values (:name)");
oci_bind_by_name($stmt, ":name", $name, 10, SQLT_CHR);
var_dump(oci_execute($stmt));

echo "Test 1 - Assign a resource to the bind variable and execute \n";
$name=$c;
var_dump(oci_execute($stmt));

echo "Test 2 - Re-bind a resource\n";
oci_bind_by_name($stmt, ":name", $c);
var_dump(oci_execute($stmt));
var_dump($c);

// Use a connection resource instead of a ROWID.
echo "Test 3 - Resource mismatch !!\n";
$stmt = oci_parse($c, "update bind_test set name='xyz' returning rowid into :r_id");
oci_bind_by_name($stmt, ":r_id", $c);
var_dump(oci_execute($stmt));

// Clean up

$drop = "drop table bind_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

echo "Done\n";

?>
--EXPECTF--
Insert value
bool(true)
Test 1 - Assign a resource to the bind variable and execute 

Warning: oci_execute(): Invalid variable used for bind in %s on line %d
bool(false)
Test 2 - Re-bind a resource

Warning: oci_bind_by_name(): Invalid variable used for bind in %s on line %d

Warning: oci_execute(): Invalid variable used for bind in %s on line %d
bool(false)
resource(%d) of type (oci8 connection)
Test 3 - Resource mismatch !!

Warning: oci_bind_by_name(): Invalid variable used for bind in %s on line %d

Warning: oci_execute(): ORA-01008: %s on line %d
bool(false)
Done
