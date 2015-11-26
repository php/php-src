--TEST--
oci_client_version()
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
?>
--FILE--
<?php

echo oci_client_version(), "\n";

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
%d.%d.%d.%d.%d
===DONE===
