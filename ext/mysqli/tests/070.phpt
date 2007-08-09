--TEST--
mysqli ping
--SKIPIF--
<?php 
require_once('skipif.inc'); 
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, $db, $port, $socket);
	var_dump($mysql->ping());
	$mysql->close();
	print "done!";
?>
--EXPECT--
bool(true)
done!