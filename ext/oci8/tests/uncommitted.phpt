--TEST--
uncommitted connection
--EXTENSIONS--
oci8
--FILE--
<?php

require __DIR__."/connect.inc";

$stmt = oci_parse($c, "select 1 from dual");
oci_execute($stmt, OCI_DEFAULT);

echo "Done\n";
?>
--EXPECT--
Done
