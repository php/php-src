--TEST--
mysqli_change_user(), MySQL 5.6+
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die(sprintf("SKIP Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket));

if (mysqli_get_server_version($link) < 50600)
	die("SKIP For MySQL >= 5.6.0");
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp	= NULL;
	$link	= NULL;

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	/* Pre 5.6: link remains useable */
	if (false !== ($tmp = @mysqli_change_user($link, $user . '_unknown_really', $passwd . 'non_empty', $db)))
		printf("[002] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = mysqli_query($link, 'SELECT 1 AS _one'))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	else
		var_dump($res->fetch_assoc());

	print "done!";
?>
--EXPECT--
[003] [2006] MySQL server has gone away
done!
