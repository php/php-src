--TEST--
ocinewcollection()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__)."/create_type.inc";

var_dump(ocinewcollection($c, $type_name));
var_dump(ocinewcollection($c, "NONEXISTENT"));

echo "Done\n";

require dirname(__FILE__)."/drop_type.inc";

?>
--EXPECTF--
object(OCI-Collection)#%d (1) {
  ["collection"]=>
  resource(%d) of type (oci8 collection)
}

Warning: ocinewcollection(): OCI-22303: type ""."NONEXISTENT" not found in %s on line %d
bool(false)
Done
