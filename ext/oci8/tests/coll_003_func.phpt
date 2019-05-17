--TEST--
collection methods
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";
require __DIR__."/create_type.inc";

$coll1 = oci_new_collection($c, $type_name);

var_dump(oci_collection_size($coll1));
var_dump(oci_collection_max($coll1));
var_dump(oci_collection_trim($coll1, 3));
var_dump(oci_collection_append($coll1, 1));
var_dump(oci_collection_element_get($coll1, 0));
var_dump(oci_collection_element_assign($coll1, 0, 2));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECTF--
int(0)
int(0)

Warning: oci_collection_trim(): OCI-22167: given trim size [3] must be less than or equal to [0] in %s on line %d
bool(false)
bool(true)
float(1)
bool(true)
Done
