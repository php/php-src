<?php
require_once('connect.inc');
if ($skip_on_connect_failure) {
	if ($socket)
		$myhost = sprintf("%s:%s", $host, $socket);
	else if ($port)
		$myhost = sprintf("%s:%s", $host, $port);

	if (!$link = @mysql_connect($myhost, $user, $passwd, true, $connect_flags))
		die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysql_errno(), mysql_error()));

	if (!@mysql_select_db($db, $link))
		die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysql_errno(), mysql_error()));

	mysql_close($link);
}
?>