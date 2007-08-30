--TEST--
oci_error() when oci_connect() fails
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

var_dump(oci_connect("some", "some", "some"));
var_dump(oci_error());

echo "Done\n";

?>
--EXPECTF--
Warning: oci_connect(): ORA-12154: TNS:could not resolve service name in %s on line %d
bool(false)
array(4) {
  ["code"]=>
  int(12154)
  ["message"]=>
  string(45) "ORA-12154: TNS:could not resolve service name"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}
Done
