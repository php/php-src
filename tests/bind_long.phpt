--TEST--
bind LONG field
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

$stmt = oci_parse($c, "drop table phptestlng");
@oci_execute($stmt);

$stmt = oci_parse($c, "create table phptestlng( id number(10), filetxt long)");
oci_execute($stmt);

$stmt = oci_parse ($c, "insert into phptestlng (id, filetxt) values (:id, :filetxt)");
$i=1;
$filetxt = file_get_contents( dirname(__FILE__)."/test.txt");

oci_bind_by_name( $stmt, ":id", $i, -1);
oci_bind_by_name( $stmt, ":filetxt", $filetxt, -1, SQLT_LNG);
oci_execute($stmt, OCI_DEFAULT);
oci_commit($c);

$stmt = oci_parse($c, "SELECT filetxt FROM phptestlng");
oci_execute($stmt);

$row = oci_fetch_row($stmt);
var_dump(md5($row[0]));
var_dump(strlen($row[0]));

$stmt = oci_parse($c, "drop table phptestlng");
oci_execute($stmt);

echo "Done\n";

?>
--EXPECT--
string(32) "5c7c34abf7ea51936785062dbfcaeddc"
int(394)
Done
