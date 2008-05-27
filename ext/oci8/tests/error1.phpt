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
Warning: oci_connect(): ORA-12154: TNS:could not resolve %s in %s on line %d
bool(false)
array(4) {
  [u"code"]=>
  int(12154)
  [u"message"]=>
  unicode(%d) "ORA-12154: TNS:could not resolve %s"
  [u"offset"]=>
  int(0)
  [u"sqltext"]=>
  unicode(0) ""
}
Done
