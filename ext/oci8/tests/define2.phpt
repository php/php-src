--TEST--
Test oci_define_by_name types
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

$stmtarray = array(
	"drop table phptestrawtable",
	"create table phptestrawtable( id number(10), fileimage raw(1000))"
);
						 
oci8_test_sql_execute($c, $stmtarray);

$stmt = oci_parse ($c, "insert into phptestrawtable (id, fileimage) values (:id, :fileimage)");
$i=1;
$fileimage = file_get_contents( dirname(__FILE__)."/test.gif");
$fileimage = substr($fileimage, 0, 300);
var_dump(md5($fileimage));

oci_bind_by_name( $stmt, ":id", $i, -1);
oci_bind_by_name( $stmt, ":fileimage", $fileimage, -1, SQLT_BIN);
oci_execute($stmt, OCI_DEFAULT);
oci_commit($c);

echo "Test 1\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phptestrawtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $fi));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($fi);
	echo "file md5:" . md5($fi) . "\n";
}

echo "Test 2\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phptestrawtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $fi));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($fi);
	echo "file md5:" . md5($fi) . "\n";
}

echo "Test 3 - test repeatability\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phptestrawtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $fi, SQLT_STR));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($fi);
	echo "file md5:" . md5($fi) . "\n";
}

echo "Test 4 - wrong type\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phptestrawtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $fi, SQLT_RSET));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($fi);
	echo "file md5:" . md5($fi) . "\n";
}

// Cleanup

$stmtarray = array(
    "drop table phptestrawtable"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECTF--
string(32) "88b274d7a257ac6f70435b83abd4e26e"
Test 1
bool(true)
string(300) "GIF89%s"
file md5:88b274d7a257ac6f70435b83abd4e26e
Test 2
bool(true)
string(300) "GIF89%s"
file md5:88b274d7a257ac6f70435b83abd4e26e
Test 3 - test repeatability
bool(true)
string(600) "47494638396178004300E66A007F82B839374728252ACCCDE2A1A4CBD3D5E7B2B4D44342588386B98283B35252729092C2C2C4DEAAACD04C4B635B5C83DDDEEC3B383C6E71A56A6D9D61638D7579B17B7EB5E5E6F0999CC68C8DC1B9BAD96B6B924E4E6B7174A97A7AA3888BBD7274A37473988E90C15A5B7EE2E3EF7B7DADA4A5D06D70A27276AC9596C8BBBDD97478AE8588BB9295C3D8D9EA9292C46466926B6E9FA5A8CE9496C52E2B2F535168B3B4D76C6A8C5C5B768A8DBF666896686A9A9C9FC8312E39AEB0D39C9CCD5556789EA1CA9699C58182AF6769973F3D50BCBEDA5E60899899C88C8EBF898ABA57587CB6B7D7D5D7E8221E206C6F9ECED0E4BFC0DC777BB47678A75F5E7D9999CC6E6F987377AE221E1FFFFFFF908E8F595657C7C6C7EEEEF5D5D4D5F6F6"
file md5:80bb3201e2a8bdcb8ab3e1a44a82bb8a
Test 4 - wrong type
bool(true)

Warning: oci_fetch(): ORA-00932: inconsistent datatypes%s on line %d
Done
