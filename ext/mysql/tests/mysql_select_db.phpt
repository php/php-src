--TEST--
mysql_select_db()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include_once "connect.inc";

$tmp    = NULL;
$link   = NULL;

if (false !== ($tmp = @mysql_select_db($link)))
	printf("[001] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
	printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

if (!is_null($tmp = @mysql_select_db($db, $link, "foo")))
	printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

/* does not make too much sense, unless we have access to at least one more database than $db */
if (!mysql_select_db($db, $link))
	printf("[004] Cannot select DB %s, [%d] %s\n", $db, mysql_errno($link), mysql_error($link));

if (!$res = mysql_query("SELECT DATABASE() AS dbname", $link))
	printf("[005] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!$row = mysql_fetch_assoc($res))
	printf("[006] [%d] %s\n", mysql_errno($link), mysql_error($link));

if ($row['dbname'] !== (string)$db)
	printf("[007] Expecting database '%s', found '%s'\n", $db, $row['dbname']);

var_dump($row['dbname']);

mysql_free_result($res);

if (mysql_select_db('mysql', $link)) {
	// Yippie, a second database to play with - that's great because mysql_select_db
	// ($db) was done by mysql__connect() already and the previous test
	// was quite useless
	if (!$res = mysql_query("SELECT DATABASE() AS dbname", $link))
		printf("[008] [%d] %s\n", mysql_errno($link), mysql_error($link));

	if (!$row = mysql_fetch_assoc($res))
		printf("[009] [%d] %s\n", mysql_errno($link), mysql_error($link));

	if (strtolower($row['dbname']) !== 'mysql')
		printf("[010] Expecting database 'mysql', found '%s'\n", $row['dbname']);

	mysql_free_result($res);
}


var_dump(mysql_select_db('I can not imagine that this database exists', $link));

mysql_close($link);

if (false !== ($tmp = mysql_select_db($db, $link)))
	printf("[012] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

print "done!\n";
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
%unicode|string%(%d) "%s"
bool(false)

Warning: mysql_select_db(): %d is not a valid MySQL-Link resource in %s on line %d
done!
