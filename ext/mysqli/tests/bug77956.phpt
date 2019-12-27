--TEST--
ensure an error is returned when mysqli.allow_local_infile is off
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');

$link = mysqli_init();
if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
	die(sprintf("skip Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error()));
}

require_once('local_infile_tools.inc');
if ($msg = check_local_infile_support($link, $engine))
	die(sprintf("skip %s, [%d] %s", $msg, $link->errno, $link->error));

?>
--INI--
mysqli.allow_local_infile=0
--FILE--
<?php
	require_once("connect.inc");
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}
	if (!$link->query("DROP TABLE IF EXISTS test")) {
		printf("[002] [%d] %s\n", $link->errno, $link->error);
	}
	if (!$link->query("CREATE TABLE test (dump1 INT UNSIGNED NOT NULL PRIMARY KEY) ENGINE=" . $engine)) {
		printf("[003] [%d] %s\n", $link->errno, $link->error);
	}
	if (FALSE == file_put_contents('bug77956.data', "waa? meukee!"))
		printf("[004] Failed to create CVS file\n");
	if (!$link->query("SELECT 1 FROM DUAL"))
		printf("[005] [%d] %s\n", $link->errno, $link->error);
	if (!$link->query("LOAD DATA LOCAL INFILE 'bug77956.data' INTO TABLE test")) {
		printf("[006] [%d] %s\n", $link->errno, $link->error);
		echo "done";
	} else {
		echo "bug";
	}
	$link->close();
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
unlink('bug77956.data');
?>
--EXPECTF--
Warning: mysqli::query(): LOAD DATA LOCAL INFILE forbidden in %s on line %d
[006] [2000] LOAD DATA LOCAL INFILE is forbidden, check mysqli.allow_local_infile
done
