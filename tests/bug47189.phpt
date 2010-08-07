--TEST--
Bug #47189 (Multiple oci_fetch_all calls) 
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

$s = oci_parse($c, "select * from dual");
oci_execute($s);
oci_fetch_all($s, $rs, 0, -1, OCI_FETCHSTATEMENT_BY_ROW);
var_dump($rs);
oci_fetch_all($s, $rs1, 0, -1, OCI_FETCHSTATEMENT_BY_ROW);
var_dump($rs1); 

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
array(1) {
  [0]=>
  array(1) {
    ["DUMMY"]=>
    string(1) "X"
  }
}
array(0) {
}
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
