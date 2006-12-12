--TEST--
ocilogon() without ORACLE_HOME set (OCIServerAttach() segfaults)
--SKIPIF--
<?php 
/* disabled for a while */
die("skip");
if (!extension_loaded('oci8')) die("skip no oci8 extension"); 
?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

if (!empty($dbase)) {
	var_dump(ocilogon($user, $password, $dbase));
}
else {
	var_dump(ocilogon($user, $password));
}
	
echo "Done\n";

?>
--EXPECTF--
Warning: ocilogon(): _oci_open_server failed, check ORACLE_HOME and NLS_LANG variables: ORA-12514: TNS:listener does not currently know of service requested in connect descriptor in %sconnect.inc on line %d

Warning: oci_connect(): _oci_open_server failed, check ORACLE_HOME and NLS_LANG variables: ORA-12514: TNS:listener does not currently know of service requested in connect descriptor in %sconnect_without_oracle_home.php on line %d
bool(false)
Done
