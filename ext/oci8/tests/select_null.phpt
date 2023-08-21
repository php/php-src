--TEST--
SELECTing NULL values
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__."/connect.inc";

$pc = oci_pconnect($user, $password, $dbase);

$stmt = oci_parse($pc, "select NULL from dual");
oci_execute($stmt);
var_dump(oci_fetch_array($stmt, OCI_RETURN_NULLS));

echo "Done\n";
?>
--EXPECTF--
array(2) {
  [0]=>
  NULL
  ["%r(NULL|EXP)%r"]=>
  NULL
}
Done
