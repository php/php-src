--TEST--
oci_new_collection() + free()
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
require __DIR__."/create_type.inc";

var_dump($coll1 = oci_new_collection($c, $type_name));

var_dump(oci_free_collection($coll1));

try {
    var_dump(oci_collection_size($coll1));
} catch(\TypeError $exception) {
    var_dump($exception->getMessage());
}

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECTF--
object(OCICollection)#%d (1) {
  ["collection"]=>
  resource(%d) of type (oci8 collection)
}
bool(true)
string(%d) "oci_collection_size(): supplied resource is not a valid oci8 collection resource"
Done
