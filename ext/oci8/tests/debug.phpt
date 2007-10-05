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

echo "The expected output will be different for different versions of Oracle\n";
echo "Done\n";

?>
--EXPECTF--
OCI8 DEBUG: OCINlsEnvironmentVariableGet at (%s:%d) 
The expected output will be different for different versions of Oracle
Done
OCI8 DEBUG: OCISessionRelease at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCISessionPoolDestroy at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d)
