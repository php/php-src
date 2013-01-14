--TEST--
LOAD DATA INFILE - open_basedir
--SKIPIF--
<?php
include_once('skipif.inc');
include_once('skipifconnectfailure.inc');


if (!$IS_MYSQLND)
	die("skip mysqlnd only, libmysql does not know about open_basedir restrictions");

if (file_exists('./simple.csv') && !unlink('./simple.csv'))
	die("skip Cannot remove previous CSV file");

if (!$fp = fopen('./simple.csv', 'w'))
	die("skip Cannot create test CSV file");

fclose($fp);
@unlink('./simple.csv');

if ($socket == "" && $host != NULL && $host != 'localhost' && $host != '.') {
	/* could be a remote TCP/IP connection. LOCAL INFILE may not work */
	if (gethostbyaddr($host) != gethostname()) {
		die("skip LOAD DATA LOCAL INFILE will fail if connecting to remote MySQL");
	}
}
?>
--FILE--
<?php
@include_once("connect.inc");
ini_set("open_basedir", __DIR__);
chdir(__DIR__);
if (!isset($db)) {
	// run-tests, I love you for not allowing me to set ini settings dynamically
	print "[006] [1148] The used command is not allowed with this MySQL version
[007] [0]
[008] LOAD DATA not run?
[010] [1148] The used command is not allowed with this MySQL version
done!";
	die();
}
require('table.inc');
mysql_close($link);
if ($socket)
	$host = sprintf("%s:%s", $host, $socket);
else if ($port)
	$host = sprintf("%s:%s", $host, $port);

if (!$link = mysql_connect($host, $user, $passwd, true, 128)) {
	printf("[001] Cannot connect using host '%s', user '%s', password '****', [%d] %s\n",
		$host, $user, $passwd,
		mysql_errno(), mysql_error());
}

if (!mysql_select_db($db, $link)) {
	printf("[002] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

if (file_exists('./simple.csv'))
	unlink('./simple.csv');

if (!$fp = fopen('./simple.csv', 'w'))
	printf("[003] Cannot open CSV file\n");

if (version_compare(PHP_VERSION, '5.9.9', '>') >= 0) {
	if (!fwrite($fp, (binary)"'97';'x';\n") ||
		!fwrite($fp, (binary)"'98';'y';\n") ||
		!fwrite($fp, (binary)"99;'z';\n")) {
		printf("[004] Cannot write CVS file '%s'\n", $file);
	}
} else {
	if (!fwrite($fp, "97;'x';\n") ||
		!fwrite($fp, "98;'y';\n") ||
		!fwrite($fp, "99;'z';\n")) {
		printf("[005] Cannot write CVS file '%s'\n", $file);
	}
}
fclose($fp);

$sql = sprintf("LOAD DATA LOCAL INFILE '%s'
			INTO TABLE test
			FIELDS TERMINATED BY ';' OPTIONALLY ENCLOSED BY '\''
			LINES TERMINATED BY '\n'",
			mysql_real_escape_string(realpath('./simple.csv'), $link));

if (!mysql_query($sql, $link))
	printf("[006] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!($res = mysql_query('SELECT label FROM test WHERE id = 97', $link)) ||
		!($row = mysql_fetch_assoc($res)) ||
		!mysql_free_result($res))
	printf("[007] [%d] '%s'\n", mysql_errno($link), mysql_error($link));

if ($row['label'] != "x")
	printf("[008] LOAD DATA not run?\n");

if (!mysql_query('DELETE FROM test', $link))
	printf("[009] [%d] %s\n", mysql_errno($link), mysql_error($link));

$sql = "LOAD DATA LOCAL INFILE '/tmp/idonotexist'
			INTO TABLE test
			FIELDS TERMINATED BY ';' OPTIONALLY ENCLOSED BY '\''
			LINES TERMINATED BY '\n'";

if (!mysql_query($sql, $link))
	printf("[010] [%d] %s\n", mysql_errno($link), mysql_error($link));

mysql_close($link);
unlink("./simple.csv");

print "done!";
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
[006] [1148] %s
[007] [0] ''
[008] LOAD DATA not run?
[010] [1148] %s
done!
