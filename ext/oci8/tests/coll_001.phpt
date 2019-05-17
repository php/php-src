--TEST--
oci_new_collection()
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";
require __DIR__."/create_type.inc";

var_dump(oci_new_collection($c, $type_name));
var_dump(oci_new_collection($c, "NONEXISTENT"));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECTF--
object(OCI-Collection)#%d (1) {
  ["collection"]=>
  resource(%d) of type (oci8 collection)
}

Warning: oci_new_collection(): OCI-22303: type ""."NONEXISTENT" not found in %s on line %d
bool(false)
Done
