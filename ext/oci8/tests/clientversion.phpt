--TEST--
oci_client_version()
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
if (preg_match('/^1[012]\./', oci_client_version()) != 1) {
    die("skip test expected to work only with Oracle 10g or greater version of client");
}
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
