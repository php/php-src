--TEST--
reusing persistent connections
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--ENV--
return "
ORACLE_HOME=".(isset($_ENV['ORACLE_HOME']) ? $_ENV['ORACLE_HOME'] : '')."
NLS_LANG=".(isset($_ENV['NLS_LANG']) ? $_ENV['NLS_LANG'] : '')."
TNS_ADMIN=".(isset($_ENV['TNS_ADMIN']) ? $_ENV['TNS_ADMIN'] : '')."
";
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
