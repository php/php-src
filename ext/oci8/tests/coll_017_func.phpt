--TEST--
collections and nulls (2)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";

error_reporting(E_ALL ^ E_DEPRECATED);

$ora_sql = "DROP TYPE ".$type_name;

$statement = oci_parse($c,$ora_sql);
@oci_execute($statement);

$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF VARCHAR(10)";

$statement = oci_parse($c,$ora_sql);
oci_execute($statement);


$coll1 = oci_new_collection($c, $type_name);

var_dump(oci_collection_append($coll1, "string"));
var_dump(oci_collection_element_assign($coll1, 0, null));
var_dump(oci_collection_element_get($coll1, 0));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECT--
bool(true)
bool(true)
NULL
Done
