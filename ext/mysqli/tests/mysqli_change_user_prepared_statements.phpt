--TEST--
mysqli_change_user() - Prepared Statement
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once('connect.inc');

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (!$stmt = mysqli_prepare($link, "SELECT 'prepared statements should be released'"))
		printf("[002] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	mysqli_change_user($link, $user, $passwd, $db);

	$wrong = null;
	if ($stmt->execute() && $stmt->bind_result($wrong) && $stmt->fetch()) {
		printf("This is wrong, because after a mysqli_change_user() %s\n", $wrong);
	} else {
		if ($stmt->errno == 0)
			printf("Error code 2013, 1243 or similar should have been set\n");
	}

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
done!