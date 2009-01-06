--TEST--
mysqli warning_count, get_warnings
--SKIPIF--
<?php 
require_once('skipif.inc'); 
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, $db, $port, $socket);

	$mysql->query("DROP TABLE IF EXISTS not_exists");

	var_dump($mysql->warning_count);

	$w = $mysql->get_warnings();

	var_dump($w->errno);
	var_dump($w->message);
	var_dump($w->sqlstate);

	$mysql->close();
	echo "done!"
?>
--EXPECTF--
int(1)
int(1051)
unicode(26) "Unknown table 'not_exists'"
unicode(5) "HY000"
done!
