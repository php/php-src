--TEST--
mysqli thread_id & kill
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

	var_dump($mysql->ping());

	var_dump($mysql->kill($mysql->thread_id));

	var_dump($mysql->ping());

	$mysql->close();

	$mysql = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

	var_dump(mysqli_ping($mysql));

	var_dump(mysqli_kill($mysql, mysqli_thread_id($mysql)));

	var_dump(mysqli_ping($mysql));

	$mysql->close();
	print "done!";
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
done!