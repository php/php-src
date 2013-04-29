--TEST--
mysql_query()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include_once("connect.inc");

$tmp    = NULL;
$link   = NULL;

if (!is_null($tmp = @mysql_query()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (false !== ($tmp = @mysql_query($link)))
	printf("[002] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');

if (NULL !== ($tmp = @mysql_query("SELECT 1 AS a", $link, "foo")))
	printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (false !== ($tmp = mysql_query('THIS IS NOT SQL', $link)))
	printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if (false !== ($tmp = mysql_query("SELECT 'this is sql but with backslash g'\g", $link)))
	printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if ((0 === mysql_errno($link)) || ('' == mysql_error($link)))
	printf("[006] mysql_errno()/mysql_error should return some error\n");

if (!$res = mysql_query("SELECT 'this is sql but with semicolon' AS valid ; ", $link))
	printf("[007] [%d] %s\n", mysql_errno($link), mysql_error($link));

var_dump(mysql_fetch_assoc($res));
mysql_free_result($res);

if (!$res = mysql_query("SELECT 'a' AS ''", $link))
	printf("[007a] [%d] %s\n", mysql_errno($link), mysql_error($link));

var_dump($tmp = mysql_fetch_assoc($res));
var_dump($tmp[""]);
mysql_free_result($res);

if (false !== ($res = mysql_query("SELECT 'this is sql but with semicolon' AS valid ; SHOW VARIABLES", $link)))
	printf("[008] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (mysql_query('DROP PROCEDURE IF EXISTS p', $link)) {
	// let's try to play with stored procedures
	if (mysql_query('CREATE PROCEDURE p(OUT ver_param VARCHAR(25)) BEGIN SELECT VERSION() INTO ver_param; END;', $link)) {
		$res = mysql_query('CALL p(@version)', $link);
		$res = mysql_query('SELECT @version AS p_version', $link);
		$tmp = mysql_fetch_assoc($res);
		if (!isset($tmp['p_version']) || ('' == $tmp['p_version'])) {
			printf("[009] Result seems wrong, dumping\n");
			var_dump($tmp);
		}
		if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && !is_unicode($tmp['p_version'])) {
			printf("[010] Expecting unicode string, dumping\n");
			var_dump($tmp);
		}
		mysql_free_result($res);
	} else {
		printf("[011] [%d] %s\n", mysql_errno($link), mysql_error($link));
	}

	mysql_query('DROP FUNCTION IF EXISTS f', $link);
	if (mysql_query('CREATE FUNCTION f( ver_param VARCHAR(25)) RETURNS VARCHAR(25) DETERMINISTIC RETURN ver_param;', $link)) {
		$res = mysql_query('SELECT f(VERSION()) AS f_version', $link);
		$tmp = mysql_fetch_assoc($res);
		if (!isset($tmp['f_version']) || ('' == $tmp['f_version'])) {
			printf("[012] Result seems wrong, dumping\n");
			var_dump($tmp);
		}
		if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && !is_unicode($tmp['f_version'])) {
			printf("[013] Expecting unicode string, dumping\n");
			var_dump($tmp);
		}
		mysql_free_result($res);
	} else {
		printf("[014] [%d] %s\n", mysql_errno($link), mysql_error($link));
	}
}

mysql_close($link);

if (false !== ($tmp = mysql_query("SELECT id FROM test", $link)))
	printf("[011] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

print "done!";
?>
--CLEAN--
<?php
require_once('connect.inc');

// connect + select_db
if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[clean] Cannot connect to the server using host=%s/%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
 	  $host, $myhost, $user, $db, $port, $socket);
}

if (!mysql_query('DROP TABLE IF EXISTS test', $link)) {
	printf("[clean] Failed to drop test table: [%d] %s\n", mysql_errno($link), mysql_error($link));
}

/* MySQL server may not support this - ignore errors */
@mysql_query('DROP PROCEDURE IF EXISTS p', $link);
@mysql_query('DROP FUNCTION IF EXISTS f', $link);

mysql_close($link);
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
array(1) {
  [%u|b%"valid"]=>
  %unicode|string%(30) "this is sql but with semicolon"
}
array(1) {
  [%u|b%""]=>
  %unicode|string%(1) "a"
}
%unicode|string%(1) "a"

Warning: mysql_query(): %d is not a valid MySQL-Link resource in %s on line %d
done!
