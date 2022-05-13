--TEST--
statement cache
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => true);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

// Note: with TimesTen, the column will be called "EXP"

require __DIR__."/connect.inc";

$pc = oci_pconnect($user, $password, $dbase);

$stmt = oci_parse($pc, "select 1+3 from dual");
oci_execute($stmt);
var_dump(oci_fetch_array($stmt));

$stmt = oci_parse($pc, "select 1+3 from dual");
oci_execute($stmt);
var_dump(oci_fetch_array($stmt));

echo "Done\n";
?>
--EXPECTF--
array(2) {
  [0]=>
  string(1) "4"
  ["%r(1\+3|EXP)%r"]=>
  string(1) "4"
}
array(2) {
  [0]=>
  string(1) "4"
  ["%r(1\+3|EXP)%r"]=>
  string(1) "4"
}
Done
