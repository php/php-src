--TEST--
mysqli_report(), change user, MySQL 5.6+
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

	$tmp    = NULL;
	$link   = NULL;

	require('table.inc');

	/*
	Internal macro MYSQL_REPORT_ERROR
	*/
	mysqli_report(MYSQLI_REPORT_ERROR);

	mysqli_change_user($link, "0123456789-10-456789-20-456789-30-456789-40-456789-50-456789-60-456789-70-456789-80-456789-90-456789", "password", $db);

	mysqli_report(MYSQLI_REPORT_OFF);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	  printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

	mysqli_change_user($link, "This might work if you accept anonymous users in your setup", "password", $db);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--

Warning: mysqli_change_user(): (%d/%d): Access denied for user '%s'@'%s' (using password: %s) in %s on line %d
done!