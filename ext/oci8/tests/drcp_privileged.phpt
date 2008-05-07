--TEST--
DRCP: privileged connect
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");
require(dirname(__FILE__)."/details.inc");
if (empty($oracle_on_localhost)) die("skip this test is unlikely to work with remote Oracle - unless an Oracle password file has been created");
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
Warning: oci_connect(): ORA-01031: insufficient privileges in %s on line %d
bool(false)

Warning: oci_connect(): ORA-01031: insufficient privileges in %s on line %d
bool(false)

Warning: oci_new_connect(): ORA-01031: insufficient privileges in %s on line %d
bool(false)

Warning: oci_new_connect(): ORA-01031: insufficient privileges in %s on line %d
bool(false)

Warning: oci_pconnect(): ORA-01031: insufficient privileges in %s on line %d
bool(false)

Warning: oci_pconnect(): ORA-01031: insufficient privileges in %s on line %d
bool(false)
Done

