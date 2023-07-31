--TEST--
collections and negative/too big element indexes
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

require __DIR__."/connect.inc";

$ora_sql = "DROP TYPE
                        ".$type_name."
           ";

$statement = oci_parse($c,$ora_sql);
@oci_execute($statement);

$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF NUMBER";

$statement = oci_parse($c,$ora_sql);
oci_execute($statement);


$coll1 = oci_new_collection($c, $type_name);

var_dump(oci_collection_append($coll1, 1));
var_dump(oci_collection_element_assign($coll1,-1,2345));
var_dump(oci_collection_element_assign($coll1,5000,2345));
var_dump(oci_collection_element_get($coll1, -1));
var_dump(oci_collection_element_get($coll1, -100));
var_dump(oci_collection_element_get($coll1, 500));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECTF--
bool(true)

Warning: oci_collection_element_assign(): OCI-22165: given index [%d] must be in the range of%s0%sto [0] in %s on line %d
bool(false)

Warning: oci_collection_element_assign(): OCI-22165: given index [5000] must be in the range of%s0%sto [0] in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
Done
