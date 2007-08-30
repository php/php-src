--TEST--
bind LONG field
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$stmt = oci_parse($c, "create table phptestlng( id number(10), fileimage long)");
oci_execute($stmt);

$stmt = oci_parse ($c, "insert into phptestlng (id, fileimage) values (:id, :fileimage)");
$i=1;
$fileimage = file_get_contents( dirname(__FILE__)."/test.gif");

oci_bind_by_name( $stmt, ":id", $i, -1);
oci_bind_by_name( $stmt, ":fileimage", $fileimage, -1, SQLT_LNG);
oci_execute($stmt, OCI_DEFAULT);
oci_commit($c);

$stmt = oci_parse($c, "SELECT fileimage FROM phptestlng");
oci_execute($stmt);

$row = oci_fetch_row($stmt);
var_dump(md5($row[0]));
var_dump(strlen($row[0]));

$stmt = oci_parse($c, "drop table phptestlng");
oci_execute($stmt);

echo "Done\n";

?>
--EXPECT--
string(32) "d04e7036e2f4221abc88fd14e960a45b"
int(2523)
Done
