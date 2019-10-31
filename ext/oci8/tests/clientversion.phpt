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
--EXPECTF--
%d.%d.%d.%d.%d
