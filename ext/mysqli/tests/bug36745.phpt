--TEST--
Bug #36745 (LOAD DATA LOCAL INFILE doesn't return correct error message)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	/*** test mysqli_connect 127.0.0.1 ***/
	$mysql = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	$mysql->query("DROP TABLE IF EXISTS litest");
	$mysql->query("CREATE TABLE litest (a VARCHAR(20))");

	$mysql->query("LOAD DATA LOCAL INFILE 'filenotfound' INTO TABLE litest");
	var_dump($mysql->error);

	$mysql->close();
	printf("Done");
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS litest"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
%s(%d) "%s"
Done
