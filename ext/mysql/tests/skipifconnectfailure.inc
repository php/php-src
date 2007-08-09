<?php
$skip_on_connect_failure  = getenv("MYSQL_TEST_SKIP_CONNECT_FAILURE") ? getenv("MYSQL_TEST_SKIP_CONNECT_FAILURE") : false;
if ($skip_on_connect_failure) {
	if ($socket)
		$myhost = sprintf("%s:%s", $host, $socket);
	else if ($port)
		$myhost = sprintf("%s:%s", $host, $port);
	
	if (!$link = @mysql_connect($myhost, $user, $passwd, true))
		die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysql_errno(), mysql_error()));

	if (!@mysql_select_db($db, $link))
		die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysql_errno(), mysql_error()));
		
	mysql_close($link);
}
?>