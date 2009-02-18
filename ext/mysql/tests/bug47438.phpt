--TEST--
Bug #47438 mysql_fetch_field ignores zero offset
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

mysql_connect("localhost", "root", "");
mysql_select_db("test");
mysql_query("DROP TABLE IF EXISTS test_47438");
mysql_query("CREATE TABLE test_47438 (a INT, b INT, c INT)");
mysql_query("INSERT INTO test_47438 VALUES (10, 11, 12), (20, 21, 22)"); 
$result = mysql_query("SELECT * FROM test_47438");
mysql_field_seek($result, 1);

$i = 0;

while($i<mysql_num_fields($result))
{
  $meta=mysql_fetch_field($result,$i);
  echo $i . "." . $meta->name . "\n";
  $i++;
}

mysql_query("DROP TABLE IF EXISTS test_47438");

?>
--EXPECT--
0.a
1.b
2.c
