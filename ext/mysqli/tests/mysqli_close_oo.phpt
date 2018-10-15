--TEST--
mysqli_close()
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

	if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!is_null($tmp = @$mysqli->close($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	$tmp = $mysqli->close();
	if (true !== $tmp)
		printf("[003] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @$mysqli->close()))
		printf("[004] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @$mysqli->query("SELECT 1")))
		printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
done!
