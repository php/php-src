--TEST--
Bug #53649 (mysql_query with "load data" unable to save result set)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}

	if (!mysql_query("DROP TABLE IF EXISTS tlocaldata", $link)) {
		printf("[002] [%d] %s\n", $link->errno, $link->error);
	}

	if (!mysql_query("CREATE TABLE tlocaldata (dump1 INT UNSIGNED NOT NULL PRIMARY KEY) ENGINE=" . $engine, $link)) {
		printf("[003] [%d] %s\n", $link->errno, $link->error);
	}

	file_put_contents('bug53649.data', "1\n2\n3\n");

	mysql_query("SELECT 1 FROM DUAL", $link);

	if (!mysql_query("LOAD DATA LOCAL INFILE 'bug53649.data' INTO TABLE tlocaldata", $link)) {
		echo "bug";
	} else {
		echo "done";
	}
	mysql_close($link);
?>
--CLEAN--
<?php
require_once('connect.inc');

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[clean] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);
}

if (!mysql_query($link, 'DROP TABLE IF EXISTS tlocaldata', $link)) {
	printf("[clean] Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

mysql_close($link);

unlink('bug53649.data');
?>
--EXPECT--
done