--TEST--
oci_internal_debug()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
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
OCI8 DEBUG: OCINlsEnvironmentVariableGet at (%s:%d) 
Done
OCI8 DEBUG: OCISessionEnd at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIServerDetach at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d)
