--TEST--
mysqli_get_server_version
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	$i = mysqli_get_server_version($link);

	$test = $i / $i;

	var_dump($test);

	mysqli_close($link);
	print "done!";
?>
--EXPECT--
int(1)
done!