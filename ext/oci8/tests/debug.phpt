--TEST--
oci_internal_debug()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--ENV--
return "
ORACLE_HOME=".(isset($_ENV['ORACLE_HOME']) ? $_ENV['ORACLE_HOME'] : '')."
NLS_LANG=".(isset($_ENV['NLS_LANG']) ? $_ENV['NLS_LANG'] : '')."
";
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

oci_internal_debug(true);

if (!empty($dbase)) {
	oci_connect($user, $password, $dbase);
}
else {
	oci_connect($user, $password);
}
	
echo "Done\n";

?>
--EXPECTF--
OCI8 DEBUG: OCINlsEnvironmentVariableGet in php_oci_do_connect_ex() (%s/oci8.c:%d) 
Done
OCI8 DEBUG: OCISessionEnd in php_oci_connection_close() (%s/oci8.c:%d) 
OCI8 DEBUG: OCIHandleFree in php_oci_connection_close() (%s/oci8.c:%d) 
OCI8 DEBUG: OCIServerDetach in php_oci_connection_close() (%s/oci8.c:%d) 
OCI8 DEBUG: OCIHandleFree in php_oci_connection_close() (%s/oci8.c:%d) 
OCI8 DEBUG: OCIHandleFree in php_oci_connection_close() (%s/oci8.c:%d) 
OCI8 DEBUG: OCIHandleFree in php_oci_connection_close() (%s/oci8.c:%d) 
OCI8 DEBUG: OCIHandleFree in php_oci_connection_close() (%s/oci8.c:%d)
