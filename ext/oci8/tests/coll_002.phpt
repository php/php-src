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
var_dump($coll1->size());

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECTF--
object(OCI-Collection)#%d (1) {
  ["collection"]=>
  resource(%d) of type (oci8 collection)
}
bool(true)

Warning: OCI-Collection::size(): supplied resource is not a valid oci8 collection resource in %s on line %d
bool(false)
Done
