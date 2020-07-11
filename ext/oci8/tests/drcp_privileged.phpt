--TEST--
DRCP: privileged connect
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension");
require(__DIR__."/connect.inc");
if (!$test_drcp) die("skip requires DRCP connection");
// Looked for :pooled in EZ connect string
if (strpos($dbase, "/") !== false && stripos($dbase, ":pooled") === false)
    die('skip DRCP test requires a DRCP pooled server connection');
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");
ob_start();
phpinfo(INFO_MODULES);
$phpinfo = ob_get_clean();
if (preg_match('/Compile-time ORACLE_HOME/', $phpinfo) !== 1) {
    // Assume building PHP with an ORACLE_HOME means the tested DB is on the same machine as PHP
    die("skip this test is unlikely to work with a remote database - unless an Oracle password file has been created");
}

preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches_sv);
// This test in Oracle 12c needs a non-CDB or the root container
if (isset($matches_sv[0]) && $matches_sv[1] >= 12) {
    $s = oci_parse($c, "select nvl(sys_context('userenv', 'con_name'), 'notacdb') as dbtype from dual");
    $r = @oci_execute($s);
    if (!$r)
        die('skip could not identify container type');
    $r = oci_fetch_array($s);
    if ($r['DBTYPE'] !== 'CDB$ROOT')
        die('skip cannot run test using a PDB');
}
?>
--INI--
oci8.privileged_connect=1
--FILE--
<?php

// Connecting as SYSDBA or SYSOPER through DRCP will give ORA-1031

require __DIR__."/details.inc";
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
