--TEST--
oci_internal_debug()
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die("skip no oci8 extension"); 
ob_start();
phpinfo(INFO_MODULES);
$phpinfo = ob_get_clean();
$iv = preg_match('/Oracle .*Version => (11\.2|12\.)/', $phpinfo);
if ($iv !== 1) {
   	die ("skip expected output only valid when using Oracle 11gR2+ client libraries");
}
?>
--FILE--
<?php

require(dirname(__FILE__)."/details.inc");

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
OCI8 DEBUG L1: Got NO cached connection at (%s:%d) 
OCI8 DEBUG: OCIEnvNlsCreate at (%s:%d) 
OCI8 DEBUG: OCIHandleAlloc at (%s:%d) 
OCI8 DEBUG: OCIHandleAlloc at (%s:%d) 
OCI8 DEBUG: OCIHandleAlloc at (%s:%d) 
OCI8 DEBUG: OCIAttrSet at (%s:%d) 
OCI8 DEBUG: OCIAttrSet at (%s:%d) 
OCI8 DEBUG: OCISessionPoolCreate at (%s:%d) 
OCI8 DEBUG: OCIAttrSet at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG L1: create_spool: (%s:%d) 
OCI8 DEBUG L1: using shared pool: (%s:%d) 
OCI8 DEBUG: OCIHandleAlloc at (%s:%d) 
OCI8 DEBUG: OCIHandleAlloc at (%s:%d) 
OCI8 DEBUG: OCIAttrSet at (%s:%d) 
OCI8 DEBUG: OCIAttrSet at (%s:%d) 
OCI8 DEBUG: OCIAttrGet at (%s:%d) 
OCI8 DEBUG: OCIAttrGet at (%s:%d) 
OCI8 DEBUG L1: (numopen=0)(numbusy=0) at (%s:%d) 
OCI8 DEBUG: OCISessionGet at (%s:%d) 
OCI8 DEBUG: OCIAttrGet at (%s:%d) 
OCI8 DEBUG: OCIAttrGet at (%s:%d) 
OCI8 DEBUG: OCIContextGetValue at (%s:%d) 
OCI8 DEBUG: OCIContextGetValue at (%s:%d) 
OCI8 DEBUG: OCIMemoryAlloc at (%s:%d) 
OCI8 DEBUG: OCIContextSetValue at (%s:%d) 
OCI8 DEBUG: OCIAttrSet at (%s:%d) 
OCI8 DEBUG L1: New Non-Persistent Connection address: (%s) at (%s:%d) 
OCI8 DEBUG L1: num_persistent=(%s:%d) 
OCI8 DEBUG: OCISessionRelease at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
Done
OCI8 DEBUG: OCISessionPoolDestroy at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d) 
OCI8 DEBUG: OCIHandleFree at (%s:%d)
