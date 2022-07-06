--TEST--
oci_connect() without ORACLE_HOME set (OCIServerAttach() segfaults)
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
ob_start();
phpinfo(INFO_MODULES);
$phpinfo = ob_get_clean();
$ov = preg_match('/Compile-time ORACLE_HOME/', $phpinfo);
if ($ov !== 1) {
    die ("skip Test only valid when OCI8 is built with an ORACLE_HOME");
}
preg_match('/^([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)/', oci_client_version(), $matches);
if (!isset($matches[0]) || !($matches[1] == 10 && $matches[2] == 2)) {
    die ("skip tests a feature that works only with Oracle 10gR2 client");
}
?>
--ENV--
ORACLE_HOME=""
--FILE--
<?php

require __DIR__."/details.inc";

if (!empty($dbase)) {
    var_dump(oci_connect($user, $password, $dbase));
}
else {
    var_dump(oci_connect($user, $password));
}

?>
--EXPECTF--
Warning: oci_connect(): OCIEnvNlsCreate() failed. There is something wrong with your system - please check that ORACLE_HOME and %s are set and point to the right directories in %s on line %d
bool(false)
