--TEST--
Bug #51647 (Certificate file without private key (pk in another file) doesn't work)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysqli.max_links = 1
mysqli.allow_persistent = Off
mysqli.max_persistent = 0
mysqli.reconnect = Off
--FILE--
<?php
	include ("connect.inc");

	$link = mysqli_init();
	if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
		printf("[002] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}
	mysqli_close($link);
	echo "closed once\n";

	$link = mysqli_init();
	if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
		printf("[002] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}
	mysqli_close($link);
	echo "closed twice\n";

	print "done!";
?>
--EXPECTF--
closed once
closed twice
done!
