--TEST--
mysqli bind_result 1
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	/*** test mysqli_connect 127.0.0.1 ***/
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	   printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test_fetch_null"))
		printf("[002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$rc = mysqli_query($link,"CREATE TABLE test_fetch_null(col1 tinyint, col2 smallint,
		col3 int, col4 bigint,
		col5 float, col6 double,
		col7 date, col8 time,
		col9 varbinary(10),
		col10 varchar(50),
		col11 char(20)) ENGINE=" . $engine);

	if (!$rc)
		printf("[003] Cannot create table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$rc = mysqli_query($link, "INSERT INTO test_fetch_null(col1,col10, col11) VALUES(1,'foo1', 1000),(2,'foo2', 88),(3,'foo3', 389789)");
	if (!$rc)
		printf("[004] Cannot insert records, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$stmt = mysqli_prepare($link, "SELECT col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11 from test_fetch_null ORDER BY col1");
	mysqli_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7, $c8, $c9, $c10, $c11);
	mysqli_execute($stmt);

	mysqli_fetch($stmt);

	$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8,$c9,$c10,$c11);

	var_dump($test);

	mysqli_stmt_close($stmt);
	@mysqli_query($link, "DROP TABLE IF EXISTS test_fetch_null");
	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_fetch_null"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
array(11) {
  [0]=>
  int(1)
  [1]=>
  NULL
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  NULL
  [5]=>
  NULL
  [6]=>
  NULL
  [7]=>
  NULL
  [8]=>
  NULL
  [9]=>
  %unicode|string%(4) "foo1"
  [10]=>
  %unicode|string%(4) "1000"
}
done!