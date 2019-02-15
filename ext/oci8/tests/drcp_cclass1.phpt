--TEST--
DRCP: Test setting connection class inline
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
require(dirname(__FILE__).'/connect.inc');
if (!$test_drcp) die("skip testing DRCP connection class only works in DRCP mode");
// Looked for :pooled in EZ connect string
if (strpos($dbase, "/") !== false && stripos($dbase, ":pooled") === false)
    die('skip DRCP test requires a DRCP pooled server connection');
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");

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
--FILE--
<?php

require(dirname(__FILE__)."/details.inc");

// Initialization

$t = time();
$cc1 = 'cc1_'.$t;
$cc2 = 'cc2_'.$t;

// Run Test

echo "Test 1\n";

ini_set('oci8.connection_class', $cc1);
$c = oci_pconnect($user, $password, $dbase);
$s = oci_parse($c, "select * from dual");
oci_execute($s);
oci_fetch_all($s, $r);
var_dump($r);

echo "Test 2\n";

ini_set('oci8.connection_class', $cc2);
$c = oci_pconnect($user, $password, $dbase);
$s = oci_parse($c, "select * from dual");
oci_execute($s);
oci_fetch_all($s, $r);
var_dump($r);

echo "Test 3\n";

$s = oci_parse($c, "select cclass_name from v\$cpool_cc_stats where cclass_name like '%.cc__$t' order by cclass_name");
oci_execute($s);
oci_fetch_all($s, $r);
var_dump($r);

// Cleanup

echo "Done\n";

?>
--EXPECTF--
Test 1
array(1) {
  ["DUMMY"]=>
  array(1) {
    [0]=>
    string(1) "X"
  }
}
Test 2
array(1) {
  ["DUMMY"]=>
  array(1) {
    [0]=>
    string(1) "X"
  }
}
Test 3
array(1) {
  ["CCLASS_NAME"]=>
  array(2) {
    [0]=>
    string(21) "%s.cc1_%d"
    [1]=>
    string(21) "%s.cc2_%d"
  }
}
Done
