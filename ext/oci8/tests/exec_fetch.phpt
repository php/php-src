--TEST--
fetch after failed oci_execute()
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

$sql = "select 2 from nonex_dual";
$stmt = oci_parse($c, $sql);

var_dump(oci_execute($stmt));
var_dump(oci_fetch_array($stmt));

echo "Done\n";
?>
--EXPECTF--
Warning: oci_execute(): ORA-00942: %s in %s on line %d
bool(false)

Warning: oci_fetch_array(): ORA-24374: %s in %s on line %d
bool(false)
Done
