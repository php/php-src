--TEST--
mysqli fetch user variable 
--SKIPIF--
<?php 
require_once('skipif.inc'); 
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	if (!mysqli_query($link, "SET @dummy='foobar'"))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$stmt = mysqli_prepare($link, "SELECT @dummy"))
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		
	mysqli_bind_result($stmt, $dummy);
	mysqli_execute($stmt);
	mysqli_fetch($stmt);

	var_dump($dummy);

	mysqli_stmt_close($stmt);
	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
unicode(6) "foobar"
done!
