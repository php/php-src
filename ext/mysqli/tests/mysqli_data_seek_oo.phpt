--TEST--
mysqli_result->data_seek()
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

	require('table.inc');

	if (!$mysqli = new mysqli($host, $user, $passwd, $db, $port, $socket))
	printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

	$res = new mysqli_result($mysqli);
	if (NULL !== ($tmp = @$res->data_seek(0)))
		printf("[002] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = $mysqli->query('SELECT * FROM test ORDER BY id LIMIT 4', MYSQLI_STORE_RESULT))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (NULL !== ($tmp = @$res->data_seek()))
		printf("[004] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @$res->data_seek($link)))
		printf("[005] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @$res->data_seek($link, $link)))
		printf("[006] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = $res->data_seek(3)))
		printf("[007] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	$row = $res->fetch_assoc();
	if (4 != $row['id'])
		printf("[008] Expecting record 4/d, got record %s/%s\n", $row['id'], $row['label']);

	if (true !== ($tmp = $res->data_seek(0)))
		printf("[009] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	$row = $res->fetch_assoc();
	if (1 != $row['id'])
		printf("[010] Expecting record 1/a, got record %s/%s\n", $row['id'], $row['label']);

	if (false !== ($tmp = $res->data_seek(4)))
		printf("[011] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	if (false !== ($tmp = $res->data_seek(-1)))
		printf("[012] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	$res->free_result();

	if (!$res = $mysqli->query('SELECT * FROM test ORDER BY id', MYSQLI_USE_RESULT))
		printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (false !== ($tmp = $res->data_seek(3)))
		printf("[014] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

	$res->free_result();

	if (NULL !== ($tmp = $res->data_seek(1)))
		printf("[015] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	$mysqli->close();

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_result::data_seek(): Function cannot be used with MYSQL_USE_RESULT in %s on line %d

Warning: mysqli_result::data_seek(): Couldn't fetch mysqli_result in %s on line %d
done!
