--TEST--
Bug #47438 (mysql_fetch_field ignores zero offset)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once('connect.inc');

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
                $host, $user, $db, $port, $socket);

mysql_select_db($db, $link);
mysql_query("DROP TABLE IF EXISTS test_47438", $link);
mysql_query("CREATE TABLE test_47438 (a INT, b INT, c INT)", $link);
mysql_query("INSERT INTO test_47438 VALUES (10, 11, 12), (20, 21, 22)", $link);
$result = mysql_query("SELECT * FROM test_47438", $link);
mysql_field_seek($result, 1);

$i = 0;

while($i<mysql_num_fields($result))
{
  $meta=mysql_fetch_field($result,$i);
  echo $i . "." . $meta->name . "\n";
  $i++;
}

mysql_query("DROP TABLE IF EXISTS test_47438", $link);

?>
--CLEAN--
<?php
require_once('connect.inc');

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
        printf("[c001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
                $host, $user, $db, $port, $socket);

if (!mysql_select_db($db, $link) ||
	!mysql_query("DROP TABLE IF EXISTS test_47438", $link))
	printf("[c002] [%d] %s\n", mysql_errno($link), mysql_error($link));

mysql_close($link);
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
0.a
1.b
2.c
