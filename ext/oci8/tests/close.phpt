--TEST--
connect/close/connect
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

oci_close($c);

oci_connect($user, $password, $dbase);

echo "Done\n";
?>
--EXPECT--
Done
