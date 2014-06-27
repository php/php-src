--TEST--
mysqli_savepoint()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

require_once('connect.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die(sprintf("Cannot connect, [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

if (!have_innodb($link))
	die(sprintf("Needs InnoDB support, [%d] %s", $link->errno, $link->error));
?>
--FILE--
<?php
	require_once("connect.inc");
	 /* {{{ proto bool mysqli_savepoint(object link, string name) */
	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_savepoint()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_savepoint($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	$name = array();
	if (!is_null($tmp = @mysqli_savepoint($link, $name)))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_savepoint($link, 'foo', $link)))
		printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (false !== ($tmp = mysqli_savepoint($link, '')))
		printf("[006] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS test'))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'CREATE TABLE test(id INT) ENGINE = InnoDB'))
		printf("[008] Cannot create test table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (true !== ($tmp = mysqli_autocommit($link, false)))
		printf("[009] Cannot turn off autocommit, expecting true, got %s/%s\n", gettype($tmp), $tmp);

	/* overrule autocommit */
	if (true !== ($tmp = mysqli_savepoint($link, 'my')))
		printf("[010] Got %s - [%d] %s\n", var_dump($tmp, true), mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'INSERT INTO test(id) VALUES (1)'))
		printf("[011] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$tmp = mysqli_rollback($link);
	if ($tmp !== true)
		printf("[012] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--

Warning: mysqli_savepoint(): Savepoint name cannot be empty in %s on line %d
done!
