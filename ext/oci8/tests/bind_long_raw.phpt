--TEST--
bind LONG RAW field
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$stmt = oci_parse($c, "create table phptestlngraw( id number(10), fileimage long raw)");
oci_execute($stmt);

$stmt = oci_parse ($c, "insert into phptestlngraw (id, fileimage) values (:id, :fileimage)");
$i=1;
$fileimage = file_get_contents( dirname(__FILE__)."/test.gif");

oci_bind_by_name( $stmt, ":id", $i, -1);
oci_bind_by_name( $stmt, ":fileimage", $fileimage, -1, SQLT_LBI);
oci_execute($stmt, OCI_DEFAULT);
oci_commit($c);

$stmt = oci_parse($c, "SELECT fileimage FROM phptestlngraw");
oci_execute($stmt);

$row = oci_fetch_row($stmt);
var_dump(md5($row[0]));
var_dump(strlen($row[0]));

$stmt = oci_parse($c, "drop table phptestlngraw");
oci_execute($stmt);

echo "Done\n";

?>
--EXPECT--
string(32) "614fcbba1effb7caa27ef0ef25c27fcf"
int(2523)
Done
