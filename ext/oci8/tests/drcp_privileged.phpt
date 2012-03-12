--TEST--
DRCP: privileged connect
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
require(dirname(__FILE__)."/details.inc");
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");
ob_start();
phpinfo(INFO_MODULES);
$phpinfo = ob_get_clean();
if (preg_match('/Compile-time ORACLE_HOME/', $phpinfo) !== 1) {
    // Assume building PHP with an ORACLE_HOME means the tested DB is on the same machine as PHP
    die("skip this test is unlikely to work with a remote database - unless an Oracle password file has been created");
}
?>
--INI--
oci8.privileged_connect=1
--FILE--
<?php

// Connecting as SYSDBA or SYSOPER through DRCP will give ORA-1031

require dirname(__FILE__)."/details.inc";
var_dump(oci_connect($user,$password,$dbase,false,OCI_SYSDBA));
var_dump(oci_connect($user,$password,$dbase,false,OCI_SYSOPER));
var_dump(oci_new_connect($user,$password,$dbase,false,OCI_SYSDBA));
var_dump(oci_new_connect($user,$password,$dbase,false,OCI_SYSOPER));
var_dump(oci_pconnect($user,$password,$dbase,false,OCI_SYSDBA));
var_dump(oci_pconnect($user,$password,$dbase,false,OCI_SYSOPER));

echo "Done\n";

?>
--EXPECTF--
Warning: oci_connect(): ORA-01031: %s in %s on line %d
bool(false)

Warning: oci_connect(): ORA-01031: %s in %s on line %d
bool(false)

Warning: oci_new_connect(): ORA-01031: %s in %s on line %d
bool(false)

Warning: oci_new_connect(): ORA-01031: %s in %s on line %d
bool(false)

Warning: oci_pconnect(): ORA-01031: %s in %s on line %d
bool(false)

Warning: oci_pconnect(): ORA-01031: %s in %s on line %d
bool(false)
Done

