--TEST--
Bug #55859 mysqli->stat property access gives error
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}
	var_dump(mysqli_stat($link) === $link->stat);
	echo "done!";
?>
--EXPECT--
bool(true)
done!