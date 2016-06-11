--TEST--
bind LONG field
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

$stmt = oci_parse($c, "drop table phptestlng");
@oci_execute($stmt);

$stmt = oci_parse($c, "create table phptestlng( id number(10), filetxt long)");
oci_execute($stmt);

echo "Test 1\n";

$stmt = oci_parse ($c, "insert into phptestlng (id, filetxt) values (:id, :filetxt)");
$i=1;
$filetxt1 = file_get_contents( dirname(__FILE__)."/test.txt");
$filetxt = str_replace("\r", "", $filetxt1);

oci_bind_by_name( $stmt, ":id", $i, -1);
oci_bind_by_name( $stmt, ":filetxt", $filetxt, -1, SQLT_LNG);
oci_execute($stmt, OCI_DEFAULT);
oci_commit($c);

$stmt = oci_parse($c, "SELECT filetxt FROM phptestlng where id = 1");
oci_execute($stmt);

$row = oci_fetch_row($stmt);
var_dump(md5($row[0]));
var_dump(strlen($row[0]));

echo "Test 2 - test multi chunk fetch\n";

$stmt = oci_parse ($c, "insert into phptestlng (id, filetxt) values (:id, :filetxt)");
$i=2;
$filetxt = str_repeat($filetxt, 600);

oci_bind_by_name( $stmt, ":id", $i, -1);
oci_bind_by_name( $stmt, ":filetxt", $filetxt, -1, SQLT_LNG);
oci_execute($stmt, OCI_DEFAULT);
oci_commit($c);

$stmt = oci_parse($c, "SELECT filetxt FROM phptestlng where id = 2");
oci_execute($stmt);

$row = oci_fetch_row($stmt);
var_dump(md5($row[0]));
var_dump(strlen($row[0]));

$stmt = oci_parse($c, "drop table phptestlng");
oci_execute($stmt);

echo "Done\n";

?>
--EXPECT--
Test 1
string(32) "5c7c34abf7ea51936785062dbfcaeddc"
int(394)
Test 2 - test multi chunk fetch
string(32) "ee2e98b977341cfb8e037066e5fcb909"
int(236400)
Done
