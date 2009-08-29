<?php
if ($skip_on_connect_failure) {
	if (!$link = @mysql_connect())
		die(sprintf("skip Can't connect to MySQL Server with default credentials - [%d] %s", mysql_errno(), mysql_error()));

	if (!@mysql_select_db($db, $link))
		die(sprintf("skip Can't connect to MySQL Server with default credentials - [%d] %s", mysql_errno(), mysql_error()));

	mysql_close($link);
}
?>
