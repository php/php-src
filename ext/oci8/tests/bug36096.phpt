--TEST--
Bug #36096 (oci_result() returns garbage after oci_fetch() failed)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__."/connect.inc";

$sql = "SELECT 'ABC' FROM DUAL WHERE 1<>1";
$stmt = oci_parse($c, $sql);

if(oci_execute($stmt, OCI_COMMIT_ON_SUCCESS)){
    var_dump(oci_fetch($stmt));
    var_dump(oci_result($stmt, 1));
    var_dump(oci_field_name($stmt, 1));
    var_dump(oci_field_type($stmt, 1));
}

echo "Done\n";

?>
--EXPECTF--
bool(false)
bool(false)
string(%r[53]%r) "%r('ABC'|EXP)%r"
string(4) "CHAR"
Done
