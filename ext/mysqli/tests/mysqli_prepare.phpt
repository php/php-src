--TEST--
mysqli_prepare()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_prepare()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_prepare($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (false !== ($tmp = @mysqli_prepare($link, false)))
		printf("[003] Expecting boolean/false, got %s\n", gettype($tmp));

	if (!$res = mysqli_query($link, "SELECT id, label FROM test", MYSQLI_USE_RESULT))
		printf("[004] [%d] %s, next test will fail\n", mysqli_errno($link), mysqli_error($link));

	if (false !== ($tmp = mysqli_prepare($link, 'SELECT id FROM test WHERE id > ?')))
		printf("[005] Expecting boolean/false, got %s, [%d] %s\n", gettype($tmp), mysqli_errno($link), mysqli_error($link));

	mysqli_free_result($res);

	if (!is_object(($stmt = mysqli_prepare($link, 'SELECT id FROM test'))) || !mysqli_stmt_execute($stmt))
		printf("[006][%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_stmt_close($stmt);


	if (!mysqli_query($link, "DROP TABLE IF EXISTS test2"))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!is_object(($stmt = mysqli_prepare($link, 'CREATE TABLE test2(id INT) ENGINE =' . $engine))) || !mysqli_stmt_execute($stmt))
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_stmt_close($stmt);


	if (!is_object(($stmt = mysqli_prepare($link, 'INSERT INTO test2(id) VALUES(?)'))))
		printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$id = 1;
	if (!mysqli_stmt_bind_param($stmt, 'i', $id) || !mysqli_stmt_execute($stmt))
		printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_stmt_close($stmt);

	if (!is_object(($stmt = mysqli_prepare($link, 'REPLACE INTO test2(id) VALUES (?)'))))
		printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$id = 2;
	if (!mysqli_stmt_bind_param($stmt, 'i', $id) || !mysqli_stmt_execute($stmt))
		printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_stmt_close($stmt);

	if (!is_object(($stmt = mysqli_prepare($link, 'UPDATE test2 SET id = ? WHERE id = ?'))))
		printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$id = 3;
	$where = 2;
	if (!mysqli_stmt_bind_param($stmt, 'ii', $id, $where) || !mysqli_stmt_execute($stmt))
		printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_stmt_close($stmt);

	if (!is_object(($stmt = mysqli_prepare($link, 'DELETE FROM test2 WHERE id = ?'))))
		printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$where = 3;
	if (!mysqli_stmt_bind_param($stmt, 'i', $where) || !mysqli_stmt_execute($stmt))
		printf("[016] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_stmt_close($stmt);

	if (!is_object(($stmt = mysqli_prepare($link, 'SET @testvar = ?'))))
		printf("[017] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$testvar = 'testvar';
	if (!mysqli_stmt_bind_param($stmt, 's', $testvar) || !mysqli_stmt_execute($stmt))
		printf("[018] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_stmt_close($stmt);

	if (!is_object(($stmt = mysqli_prepare($link, "DO GET_LOCK('testlock', 1)"))))
		printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	mysqli_stmt_close($stmt);

	if (!is_object(($stmt = mysqli_prepare($link, 'SELECT id, @testvar FROM test2'))))
		printf("[020] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$id = $testvar = null;
	if (!mysqli_stmt_execute($stmt) || !mysqli_stmt_bind_result($stmt, $id, $testvar))
		printf("[021] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	while (mysqli_stmt_fetch($stmt)) {
		if (('testvar' !== $testvar) || (1 !== $id))
			printf("[022] Expecting 'testvar'/1, got %s/%s. [%d] %s\n",
				$testvar, $id, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	}

	var_dump(mysqli_stmt_prepare($stmt, 'SELECT 1; SELECT 2'));

	mysqli_stmt_close($stmt);

	if (!is_null($tmp = @mysqli_stmt_prepare($link, 'SELECT id FROM test', 'foo')))
		printf("[023] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	if (!is_null($tmp = @mysqli_stmt_prepare($link, 'SELECT id FROM test')))
		printf("[024] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

if (!mysqli_query($link, "DROP TABLE IF EXISTS test2"))
	printf("[c003] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
bool(false)
done!
