--TEST--
Test collection Oracle error handling collections and numbers (2)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$ora_sql = "DROP TYPE ".$type_name;;
$statement = oci_parse($c,$ora_sql);
@oci_execute($statement);


echo "Test 0\n";
$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF BLOB";
$statement = oci_parse($c,$ora_sql);
oci_execute($statement);

$coll1 = oci_new_collection($c, $type_name);

var_dump($coll1->append('a long string'));              // invalid type for append
var_dump($coll1->assignElem(1, 'a long string')); 	// invalid type for assignelem()
var_dump($coll1->getElem(0));

require dirname(__FILE__)."/drop_type.inc";

echo "Test 1\n";
$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF NUMBER";
$statement = oci_parse($c,$ora_sql);
oci_execute($statement);

$coll1 = oci_new_collection($c, $type_name);

var_dump($coll1->assignElem(1, null));			// invalid location for null
var_dump($coll1->getElem(0));

echo "Test 2\n";
var_dump($coll1->assignElem(1, 1234));  		// invalid location for number
var_dump($coll1->getElem(0));

require dirname(__FILE__)."/drop_type.inc";

echo "Test 3\n";
$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF VARCHAR2(1)";
$statement = oci_parse($c,$ora_sql);
oci_execute($statement);

$coll1 = oci_new_collection($c, $type_name);

var_dump($coll1->assignElem(1, 'abc')); 		// invalid location for string
var_dump($coll1->getElem(0));

require dirname(__FILE__)."/drop_type.inc";

echo "Test 4\n";
$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF DATE";
$statement = oci_parse($c,$ora_sql);
oci_execute($statement);

$coll1 = oci_new_collection($c, $type_name);

var_dump($coll1->append(1));                   		// invalid date format
var_dump($coll1->assignElem(1, '01-JAN-06'));  		// invalid location for date
var_dump($coll1->getElem(0));

require dirname(__FILE__)."/drop_type.inc";

echo "Done\n";

?>
--EXPECTF--
Test 0

Notice: OCI-Collection::append(): Unknown or unsupported type of element: 113 in %s on line %d
bool(false)

Notice: OCI-Collection::assignelem(): Unknown or unsupported type of element: 113 in %s on line %d
bool(false)
bool(false)
Test 1

Warning: OCI-Collection::assignelem(): OCI-22165: given index [1] must be in the range of %s in %s on line %d
bool(false)
bool(false)
Test 2

Warning: OCI-Collection::assignelem(): OCI-22165: given index [1] must be in the range of %s in %s on line %d
bool(false)
bool(false)
Test 3

Warning: OCI-Collection::assignelem(): OCI-22165: given index [1] must be in the range of %s in %s on line %d
bool(false)
bool(false)
Test 4

Warning: OCI-Collection::append(): OCI-01840: input value not long enough for date format in %s on line %d
bool(false)

Warning: OCI-Collection::assignelem(): OCI-22165: given index [1] must be in the range of %s in %s on line %d
bool(false)
bool(false)
Done
