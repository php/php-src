--TEST--
PECL Bug #8816 (issue in php_oci_statement_fetch with more than one piecewise column)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$create_1 = "CREATE TABLE t1 (id INTEGER, l1 LONG)";
$create_2 = "CREATE TABLE t2 (id INTEGER, l2 LONG)";
$drop_1 = "DROP TABLE t1";
$drop_2 = "DROP TABLE t2";

$s1 = oci_parse($c, $drop_1);
$s2 = oci_parse($c, $drop_2);
@oci_execute($s1);
@oci_execute($s2);

$s1 = oci_parse($c, $create_1);
$s2 = oci_parse($c, $create_2);
oci_execute($s1);
oci_execute($s2);

$values = array("1234567890111111111", "122222222222222", "985456745674567654567654567654", "123456789", "987654321");

$i = 0;
foreach ($values as $val) {
	$i++;
	$insert = "INSERT INTO t1 VALUES($i, ".$val.")";
	$s = oci_parse($c, $insert);
	oci_execute($s);
}

foreach ($values as $val) {
	$insert = "INSERT INTO t2 VALUES($i, ".$val.")";
	$s = oci_parse($c, $insert);
	oci_execute($s);
	$i--;
}

$query ="
SELECT
  t1.l1, t2.l2
FROM
t1, t2
WHERE
t1.id = t2.id
ORDER BY t1.id ASC
";

$sth = oci_parse($c, $query);
oci_execute($sth);

while ( $row = oci_fetch_assoc($sth) ) {
	var_dump($row);
}

$s1 = oci_parse($c, $drop_1);
$s2 = oci_parse($c, $drop_2);
@oci_execute($s1);
@oci_execute($s2);

echo "Done\n";

?>
--EXPECT--
array(2) {
  ["L1"]=>
  string(19) "1234567890111111111"
  ["L2"]=>
  string(9) "987654321"
}
array(2) {
  ["L1"]=>
  string(15) "122222222222222"
  ["L2"]=>
  string(9) "123456789"
}
array(2) {
  ["L1"]=>
  string(30) "985456745674567654567654567654"
  ["L2"]=>
  string(30) "985456745674567654567654567654"
}
array(2) {
  ["L1"]=>
  string(9) "123456789"
  ["L2"]=>
  string(15) "122222222222222"
}
array(2) {
  ["L1"]=>
  string(9) "987654321"
  ["L2"]=>
  string(19) "1234567890111111111"
}
Done
