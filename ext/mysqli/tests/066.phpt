--TEST--
function test: mysqli_warning object
--SKIPIF--
<?php 
require_once('skipif.inc'); 
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

	include "connect.inc";

	/*** test mysqli_connect 127.0.0.1 ***/
	$mysql = new mysqli($host, $user, $passwd, $db, $port, $socket);

	$mysql->query("DROP TABLE IF EXISTS test_warnings");

	$mysql->query("CREATE TABLE test_warnings (a int not null) ENGINE=myisam");

	$mysql->query("INSERT INTO test_warnings VALUES (1),(2),(NULL)");

	if (($warning = $mysql->get_warnings())) {
		do {
			printf("Warning\n");
		} while ($warning->next());
	}

	$mysql->close();
	print "done!";
?>
--EXPECT--
Warning
done!
