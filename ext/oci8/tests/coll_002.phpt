--TEST--
oci_new_collection() + free()
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
var_dump($coll1->free());

try {
    var_dump($coll1->size());
} catch (TypeError $error) {
    var_dump($error->getMessage());
}

var_dump(oci_new_collection($c, "NONEXISTENT"));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECTF--
object(OCICollection)#%d (1) {
  ["collection"]=>
  resource(%d) of type (oci8 collection)
}
bool(true)
string(%d) "OCICollection::size(): supplied resource is not a valid oci8 collection resource"

Warning: oci_new_collection(): OCI-22303: type ""."NONEXISTENT" not found in %s on line %d
bool(false)
Done
