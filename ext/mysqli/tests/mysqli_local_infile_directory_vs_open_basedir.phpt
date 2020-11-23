--TEST--
mysqli.local_infile_directory vs open_basedir
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die("skip Cannot connect to MySQL");

include_once("local_infile_tools.inc");
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
	require_once("connect.inc");

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}

	echo "done";
?>
--CLEAN--
<?php
require_once('connect.inc');

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[clean] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);
}

if (!$link->query($link, 'DROP TABLE IF EXISTS test')) {
	printf("[clean] Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

$link->close();
?>
--EXPECTF--
Warning: mysqli_connect(): open_basedir restriction in effect. File(%s) in %s

Warning: mysqli_connect(%s): Failed to open directory: Operation not permitted in %s
done
