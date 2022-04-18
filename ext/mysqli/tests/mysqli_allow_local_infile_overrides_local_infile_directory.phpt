--TEST--
mysqli.allow_local_infile overrides mysqli.local_infile_directory
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';

if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
}

include_once "local_infile_tools.inc";
if ($msg = check_local_infile_allowed_by_server($link))
    die(sprintf("skip %s, [%d] %s", $msg, $link->errno, $link->error));

mysqli_close($link);
?>
--INI--
open_basedir={PWD}
mysqli.allow_local_infile=1
mysqli.local_infile_directory={PWD}/foo/bar
--FILE--
<?php
	require_once("connect.inc");

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}

	if (!$link->query("DROP TABLE IF EXISTS test")) {
		printf("[002] [%d] %s\n", $link->errno, $link->error);
	}

	if (!$link->query("CREATE TABLE test (id INT UNSIGNED NOT NULL PRIMARY KEY) ENGINE=" . $engine)) {
		printf("[003] [%d] %s\n", $link->errno, $link->error);
	}

	$filepath = str_replace('\\', '/', __DIR__.'/foo/foo.data');
	if (!$link->query("LOAD DATA LOCAL INFILE '".$filepath."' INTO TABLE test")) {
		printf("[004] [%d] %s\n", $link->errno, $link->error);
	}

	if ($res = mysqli_query($link, 'SELECT COUNT(id) AS num FROM test')) {
		$row = mysqli_fetch_assoc($res);
		mysqli_free_result($res);

		$row_count = $row['num'];
		$expected_row_count = 3;
		if ($row_count != $expected_row_count) {
			printf("[005] %d != %d\n", $row_count, $expected_row_count);
		}
	} else {
		printf("[006] [%d] %s\n", $link->errno, $link->error);
	}

	$link->close();
	echo "done";
?>
--CLEAN--
<?php
require_once('connect.inc');

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[clean] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);
}

if (!$link->query('DROP TABLE IF EXISTS test')) {
	printf("[clean] Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

$link->close();
?>
--EXPECT--
done
