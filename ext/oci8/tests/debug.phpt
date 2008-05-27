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
OCI8 DEBUG L1: Got a cached connection: (%s) at (%s:%d) 
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
