--TEST--
oci_client_version() for Oracle 9.2 client libraries
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
if (preg_match('/Unknown/', oci_client_version()) != 1) {
    die("skip test expected to work only with Oracle 9gR2 client libraries");
}
?>
--FILE--
<?php

echo oci_client_version(), "\n";

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Unknown
===DONE===
