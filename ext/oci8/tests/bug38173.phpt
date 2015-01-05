--TEST--
Bug #38173 (Freeing nested cursors causes OCI8 to segfault)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$create_1 = "CREATE TABLE t1 (id INTEGER)";
$create_2 = "CREATE TABLE t2 (id INTEGER)";
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

for($i=0; $i < 5; $i++) {
	$insert = "INSERT INTO t1 VALUES(".$i.")";
	$s = oci_parse($c, $insert);
	oci_execute($s);
}

for($i=0; $i < 5; $i++) {
	$insert = "INSERT INTO t2 VALUES(".$i.")";
	$s = oci_parse($c, $insert);
	oci_execute($s);
}

$query ="
SELECT
  t1.*,
  CURSOR( SELECT * FROM t2 ) as cursor
FROM
  t1
";

$sth = oci_parse($c, $query);
oci_execute($sth);

// dies on oci_free_statement on 2nd pass through loop
while ( $row = oci_fetch_assoc($sth) ) {
  print "Got row!\n";
  var_dump(oci_execute($row['CURSOR']));
  var_dump(oci_free_statement($row['CURSOR']));
}

$s1 = oci_parse($c, $drop_1);
$s2 = oci_parse($c, $drop_2);
@oci_execute($s1);
@oci_execute($s2);

echo "Done\n";

?>
--EXPECT--
Got row!
bool(true)
bool(true)
Got row!
bool(true)
bool(true)
Got row!
bool(true)
bool(true)
Got row!
bool(true)
bool(true)
Got row!
bool(true)
bool(true)
Done
