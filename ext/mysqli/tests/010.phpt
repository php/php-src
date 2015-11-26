--TEST--
mysqli fetch float values
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

	if (!mysqli_query($link, "SET sql_mode=''"))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch"))
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$rc = mysqli_query($link, "CREATE TABLE test_bind_fetch(c1 float(3),
													 c2 float,
													 c3 float unsigned,
													 c4 float,
													 c5 float,
													 c6 float,
													 c7 float(10) unsigned) ENGINE=" . $engine);
	if (!$rc)
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));


	mysqli_query($link, "INSERT INTO test_bind_fetch (c1,c2,c3,c4,c5,c6,c7) VALUES (3.1415926535,-0.000001, -5, 999999999999,
											sin(0.6), 1.00000000000001, 888888888888888)");

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch");
	mysqli_stmt_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7);
	mysqli_stmt_execute($stmt);
	mysqli_stmt_fetch($stmt);

	$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7);

	var_dump($test);

	mysqli_stmt_close($stmt);
	mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch");
	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
array(7) {
  [0]=>
  float(3.14159)
  [1]=>
  float(-1.0E-6)
  [2]=>
  float(0)
  [3]=>
  float(1.0E+12)
  [4]=>
  float(0.564642)
  [5]=>
  float(1)
  [6]=>
  float(8.88889E+14)
}
done!
