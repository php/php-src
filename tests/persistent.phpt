--TEST--
reusing persistent connections
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php
		
require dirname(__FILE__)."/connect.inc";

var_dump(oci_pconnect($user, $password, $dbase));
var_dump(oci_pconnect($user, $password, $dbase));
var_dump(oci_pconnect($user, $password, $dbase));
var_dump(oci_connect($user, $password, $dbase));
var_dump(oci_connect($user, $password, $dbase));
var_dump(oci_connect($user, $password, $dbase));

echo "Done\n";
?>
--EXPECTF--	
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
Done
