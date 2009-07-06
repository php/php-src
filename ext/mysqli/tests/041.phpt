--TEST--
function test: mysqli_warning_count()
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
	mysqli_select_db($link, $db);

	mysqli_query($link, "DROP TABLE IF EXISTS test_warnings");
	mysqli_query($link, "DROP TABLE IF EXISTS test_warnings");

	var_dump(mysqli_warning_count($link));

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
include "connect.inc";
if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test_warnings"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
int(1)
done!