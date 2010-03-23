--TEST--
statement cache
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

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
  ["1+3"]=>
  string(1) "4"
}
array(2) {
  [0]=>
  string(1) "4"
  ["1+3"]=>
  string(1) "4"
}
Done
