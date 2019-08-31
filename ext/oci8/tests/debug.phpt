--TEST--
oci_internal_debug()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__."/details.inc");

oci_internal_debug(true);

if (!empty($dbase)) {
	oci_connect($user, $password, $dbase);
}
else {
	oci_connect($user, $password);
}

oci_internal_debug(false);

?>
===DONE===
--EXPECT--
===DONE===
