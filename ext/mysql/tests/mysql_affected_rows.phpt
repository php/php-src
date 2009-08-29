--TEST--
mysql_affected_rows()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('skipifdefaultconnectfailure.inc');
?>
--FILE--
<?php
include_once("connect.inc");

$tmp    = NULL;
$link   = NULL;

if (0 !== ($tmp = @mysql_affected_rows()))
	printf("[001] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

if (null !== ($tmp = @mysql_affected_rows($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!is_null($tmp = @mysql_affected_rows($link, $link)))
	printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
	printf("[004] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

if (-1 !== ($tmp = mysql_affected_rows($link)))
	printf("[005] Expecting int/-1, got %s/%s. [%d] %s\n",
		gettype($tmp), $tmp, mysql_errno($link), mysql_error($link));

if (!mysql_query('DROP TABLE IF EXISTS test', $link))
	printf("[006] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!mysql_query('CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE = ' . $engine, $link))
	printf("[007] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!mysql_query("INSERT INTO test(id, label) VALUES (1, 'a')", $link))
	printf("[008] [%d] %s\n",  mysql_errno($link), mysql_error($link));

if (1 !== ($tmp = mysql_affected_rows($link)))
	printf("[010] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

// ignore INSERT error, NOTE: command line returns 0, affected_rows returns -1 as documented
@mysql_query("INSERT INTO test(id, label) VALUES (1, 'a')", $link);
if (-1 !== ($tmp = mysql_affected_rows($link)))
	printf("[011] Expecting int/-1, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query("INSERT INTO test(id, label) VALUES (1, 'a') ON DUPLICATE KEY UPDATE id = 4", $link))
	printf("[012] [%d] %s\n",  mysql_errno($link), mysql_error($link));

if (2 !== ($tmp = mysql_affected_rows($link)))
	printf("[013] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query("INSERT INTO test(id, label) VALUES (2, 'b'), (3, 'c')", $link))
	printf("[014] [%d] %s\n",  mysql_errno($link), mysql_error($link));

if (2 !== ($tmp = mysql_affected_rows($link)))
	printf("[015] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query("INSERT IGNORE INTO test(id, label) VALUES (1, 'a')", $link)) {
	printf("[016] [%d] %s\n",  mysql_errno($link), mysql_error($link));
}

if (1 !== ($tmp = mysql_affected_rows($link)))
	printf("[017] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query("INSERT INTO test(id, label) SELECT id + 10, label FROM test", $link))
	printf("[018] [%d] %s\n",  mysql_errno($link), mysql_error($link));

if (4 !== ($tmp = mysql_affected_rows($link)))
	printf("[019] Expecting int/4, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query("REPLACE INTO test(id, label) values (4, 'd')", $link))
	printf("[020] [%d] %s\n",  mysql_errno($link), mysql_error($link));

if (2 !== ($tmp = mysql_affected_rows($link)))
	printf("[021] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query("REPLACE INTO test(id, label) values (5, 'e')", $link))
	printf("[022] [%d] %s\n",  mysql_errno($link), mysql_error($link));

if (1 !== ($tmp = mysql_affected_rows($link)))
	printf("[023] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query("UPDATE test SET label = 'a' WHERE id = 2", $link))
	printf("[024] [%d] %s\n",  mysql_errno($link), mysql_error($link));

if (1 !== ($tmp = mysql_affected_rows($link)))
	printf("[025] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query("UPDATE test SET label = 'a' WHERE id = 2", $link)) {
	printf("[025] [%d] %s\n",  mysql_errno($link), mysql_error($link));
}

if (0 !== ($tmp = mysql_affected_rows($link)))
	printf("[026] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query("UPDATE test SET label = 'a' WHERE id = 100", $link)) {
	printf("[025] [%d] %s\n",  mysql_errno($link), mysql_error($link));
}

if (0 !== ($tmp = mysql_affected_rows($link)))
	printf("[026] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

if (0 !== ($tmp = mysql_affected_rows()))
	printf("[027] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

if (!mysql_query('DROP TABLE IF EXISTS test', $link))
	printf("[028] [%d] %s\n", mysql_errno($link), mysql_error($link));

mysql_close($link);

if (false !== ($tmp = @mysql_affected_rows($link)))
	printf("[029] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
done!
