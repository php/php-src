--TEST--
oci_client_version()
--EXTENSIONS--
oci8
--FILE--
<?php

echo oci_client_version(), "\n";

?>
--EXPECTF--
%d.%d.%d.%d.%d
