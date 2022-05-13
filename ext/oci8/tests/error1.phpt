--TEST--
oci_error() when oci_connect() fails
--EXTENSIONS--
oci8
--FILE--
<?php

var_dump(oci_connect("some", "some", "some"));
var_dump(oci_error());

echo "Done\n";

?>
--EXPECTF--
Warning: oci_connect(): ORA-12154: %s in %s on line %d
bool(false)
array(4) {
  ["code"]=>
  int(12154)
  ["message"]=>
  string(%d) "ORA-12154: %s"
  ["offset"]=>
  int(0)
  ["sqltext"]=>
  string(0) ""
}
Done
