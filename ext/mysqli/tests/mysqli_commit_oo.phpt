--TEST--
mysqli_commit()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket)) {
	die(sprintf("skip Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket));
}

if (!$res = $mysqli->query("SHOW VARIABLES LIKE 'have_innodb'")) {
	die(sprintf("skip Cannot fetch have_innodb variable\n"));
}

$row = $res->fetch_row();
$res->free();
$mysqli->close();

if ($row[1] == "DISABLED" || $row[1] == "NO") {
	die(sprintf("skip Innodb support is not installed or enabled."));
}
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	$mysqli = new mysqli();
	if (!is_null($tmp = @$mysqli->commit()))
		printf("[013] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

	if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!is_null($tmp = @$mysqli->commit($link)))
		printf("[002] Expecting NULL/NULL, got %s/%s, [%d] %s\n",
			gettype($tmp), $tmp, $mysqli->errno, $mysqli->error);

	if (true !== ($tmp = $mysqli->commit()))
		printf("[014] Expecting boolean/true got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = $mysqli->autocommit(false)))
		printf("[003] Cannot turn off autocommit, expecting true, got %s/%s\n", gettype($tmp), $tmp);

	if (!$mysqli->query('DROP TABLE IF EXISTS test'))
		printf("[004] [%d] %s\n", $mysqli->errno, $mysqli->error);

	if (!$mysqli->query('CREATE TABLE test(id INT) ENGINE = InnoDB'))
		printf("[005] Cannot create test table, [%d] %s\n", $mysqli->errno, $mysqli->error);

	if (!$mysqli->query('INSERT INTO test(id) VALUES (1)'))
		printf("[006] [%d] %s\n", $mysqli->errno, $mysqli->error);

	$tmp = $mysqli->commit();
	if ($tmp !== true)
		printf("[007] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (!$mysqli->query('ROLLBACK'))
		printf("[008] [%d] %s\n", $mysqli->errno, $mysqli->error);

	if (!$res = $mysqli->query('SELECT COUNT(*) AS num FROM test'))
		printf("[009] [%d] %s\n", $mysqli->errno, $mysqli->error);
	$tmp = $res->fetch_assoc();
	if (1 != $tmp['num'])
		printf("[010] Expecting 1 row in table test, found %d rows\n", $tmp['num']);
	$res->free();

	if (!$mysqli->query('DROP TABLE IF EXISTS test'))
		printf("[011] [%d] %s\n", $mysqli->errno, $mysqli->error);

	$mysqli->close();

	if (NULL !== ($tmp = @$mysqli->commit()))
		printf("[012] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!