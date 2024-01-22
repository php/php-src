--TEST--
mysqli.local_infile_directory vs open_basedir
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
mysqli.allow_local_infile=0
mysqli.local_infile_directory={PWD}/../
--FILE--
<?php
	require_once 'connect.inc';

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
	} else {
		printf("[005] bug! should not happen - operation not permitted expected\n");
	}

	echo "done";
?>
--CLEAN--
<?php
require_once 'connect.inc';

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[clean] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);
}

if (!$link->query('DROP TABLE IF EXISTS test')) {
	printf("[clean] Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

$link->close();
?>
--EXPECTF--
Warning: mysqli_connect(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (%s) in %s on line %d
[004] [2068] LOAD DATA LOCAL INFILE %s
done
