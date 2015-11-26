--TEST--
function test: mysqli_get_proto_info
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	$pinfo = mysqli_get_proto_info($link);

	var_dump($pinfo);

	mysqli_close($link);
	print "done!";
?>
--EXPECT--
int(10)
done!