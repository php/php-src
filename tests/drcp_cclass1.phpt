--TEST--
DRCP: Test setting connection class inline
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die ("skip no oci8 extension"); 
require(dirname(__FILE__)."/details.inc");
if (!$test_drcp) die("skip testing DRCP connection class only works in DRCP mode");
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user"); 
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
