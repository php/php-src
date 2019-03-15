--TEST--
Bug #47189 (Multiple oci_fetch_all calls)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs: different error handling for this undefined behavior
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

echo "Test 1\n";

$s = oci_parse($c, "select * from dual");
oci_execute($s);
oci_fetch_all($s, $rs, 0, -1, OCI_FETCHSTATEMENT_BY_ROW);
var_dump($rs);
oci_fetch_all($s, $rs1, 0, -1, OCI_FETCHSTATEMENT_BY_ROW);
var_dump($rs1);

echo "Test 2\n";

$s = oci_parse($c, "select * from dual");
oci_execute($s);
oci_fetch_all($s, $rs, 0, 1, OCI_FETCHSTATEMENT_BY_ROW);
var_dump($rs);
oci_fetch_all($s, $rs1, 0, 1, OCI_FETCHSTATEMENT_BY_ROW);
var_dump($rs1);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
array(1) {
  [0]=>
  array(1) {
    ["DUMMY"]=>
    string(1) "X"
  }
}
array(0) {
}
Test 2
array(1) {
  [0]=>
  array(1) {
    ["DUMMY"]=>
    string(1) "X"
  }
}

Warning: oci_fetch_all(): ORA-01002: %s in %s on line %d
array(0) {
}
===DONE===
