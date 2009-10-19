--TEST--
mysqli fetch mixed values
--INI--
precision=12
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bind_result"))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$rc = mysqli_query($link, "CREATE TABLE test_bind_result(c1 tinyint, c2 smallint,
														c3 int, c4 bigint,
														c5 float, c6 double,
														c7 varbinary(10),
														c8 varchar(50)) ENGINE=" . $engine);
	if (!$rc)
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$rc = mysqli_query($link,"INSERT INTO test_bind_result VALUES(19,2999,3999,4999999,
															  2345.6,5678.89563,
															  'foobar','mysql rulez')");
	if (!$rc)
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_result");
	mysqli_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7, $c8);
	mysqli_execute($stmt);
	mysqli_fetch($stmt);

	$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7,$c8);

	var_dump($test);

	mysqli_stmt_close($stmt);
	mysqli_query($link, "DROP TABLE IF EXISTS test_bind_result");
	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bind_result"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
array(8) {
  [0]=>
  int(19)
  [1]=>
  int(2999)
  [2]=>
  int(3999)
  [3]=>
  int(4999999)
  [4]=>
  float(2345.60009766)
  [5]=>
  float(5678.89563)
  [6]=>
  string(6) "foobar"
  [7]=>
  %unicode|string%(11) "mysql rulez"
}
done!