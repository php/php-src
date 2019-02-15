--TEST--
Bug #38161 (oci_bind_by_name() returns garbage when Oracle didn't set the variable)
--SKIPIF--
<?php if (!extension_loaded("oci8")) print "skip"; ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

$query = "begin if false then :bv := 1; end if; end;";
$stid = oci_parse($c, $query);
oci_bind_by_name($stid, ":bv", $bv, 22);
oci_execute($stid, OCI_DEFAULT);

var_dump($bv);
unset($bv);

$query = "begin if false then :bv := 1; end if; end;";
$stid = oci_parse($c, $query);
oci_bind_by_name($stid, ":bv", $bv, 22, SQLT_INT);
oci_execute($stid, OCI_DEFAULT);

var_dump($bv);

echo "Done\n";
?>
--EXPECT--
NULL
int(0)
Done
