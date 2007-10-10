<?php
$skip_on_connect_failure  = getenv("MYSQL_TEST_SKIP_CONNECT_FAILURE") ? getenv("MYSQL_TEST_SKIP_CONNECT_FAILURE") : false;
if ($skip_on_connect_failure) {
	include_once('connect.inc');
	$link = @mysqli_connect($host, $user, $passwd, $db, $port, $socket);
	if (!is_object($link))
		die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
	mysqli_close($link);
}
?>
