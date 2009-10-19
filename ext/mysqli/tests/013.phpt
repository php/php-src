--TEST--
mysqli fetch mixed / mysql_query (may fail when using 4.1 library with 5.x server)
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
														c5 decimal(4,2), c6 double,
														c7 varbinary(10),
														c8 varchar(10)) ENGINE=" . $engine);
	if (!$rc)
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "INSERT INTO test_bind_result VALUES(120,2999,3999,54,
															  2.6,58.89,
															  '206','6.7')"))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_result");

	$c = array(0,0,0,0,0,0,0,0);
	$b_res= mysqli_bind_result($stmt, $c[0], $c[1], $c[2], $c[3], $c[4], $c[5], $c[6], $c[7]);
	mysqli_execute($stmt);
	mysqli_fetch($stmt);
	mysqli_fetch($stmt);
	mysqli_stmt_close($stmt);

	$result = mysqli_query($link, "select * from test_bind_result");
	$d = mysqli_fetch_row($result);
	mysqli_free_result($result);

	$test = "";
	for ($i=0; $i < count($c); $i++)
		$test .= ($c[$i] == $d[$i]) ? "1" : "0";
	if ($test == "11111111")
		echo "ok\n";
	else if ($b_res == FALSE && mysqli_get_client_version() > 40100 && mysqli_get_client_version() < 50000 &&
				 mysqli_get_server_version($link) > 50000)
		echo "error (4.1 library with 5.x server)";
	else
		echo "error";

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
ok
done!