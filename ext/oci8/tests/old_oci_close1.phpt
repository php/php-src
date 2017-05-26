--TEST--
oci8.old_oci_close_semantics Off
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--INI--
oci8.old_oci_close_semantics=0
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

var_dump($c);
var_dump(oci_close($c));
var_dump(oci_parse($c, "select 1 from dual"));

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
bool(true)

Warning: oci_parse(): supplied resource is not a valid oci8 connection resource in %sold_oci_close1.php on line %d
bool(false)
Done
