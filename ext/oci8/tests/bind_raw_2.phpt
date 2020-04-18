--TEST--
bind RAW field with OCI_B_BIN
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";

$stmt = oci_parse($c, "create table phptestrawtable( id number(10), fileimage raw(1000))");
oci_execute($stmt);

$stmt = oci_parse ($c, "insert into phptestrawtable (id, fileimage) values (:id, :fileimage)");
$i=1;
$fileimage = file_get_contents( __DIR__."/test.gif");
$fileimage = substr($fileimage, 0, 300);

oci_bind_by_name( $stmt, ":id", $i, -1);
oci_bind_by_name( $stmt, ":fileimage", $fileimage, -1, OCI_B_BIN);
oci_execute($stmt, OCI_DEFAULT);
oci_commit($c);

$stmt = oci_parse($c, "SELECT fileimage FROM phptestrawtable");
oci_execute($stmt);

$row = oci_fetch_row($stmt);
var_dump(md5($row[0]));
var_dump(strlen($row[0]));

$stmt = oci_parse($c, "drop table phptestrawtable");
oci_execute($stmt);

echo "Done\n";

?>
--EXPECT--
string(32) "88b274d7a257ac6f70435b83abd4e26e"
int(300)
Done
