--TEST--
mysql_list_fields()
--SKIPIF--
<?php
require_once 'skipif.inc';
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$tmp    = NULL;
$link   = NULL;

require 'table.inc';

if (!$res = mysql_list_fields($db, 'test', $link))
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (2 !== ($num = mysql_num_fields($res)))
	printf("[004] Expecting two fields from 'test', got %d. [%d] %s\n", $num, mysql_errno($link), mysql_error($link));

mysql_free_result($res);

if (!mysql_query("DROP TABLE IF EXISTS test2", $link))
	printf("[005] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!$res = @mysql_list_fields($db, 'test2', $link))
	printf("[006] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!$res = mysql_list_fields($db, 'test', $link))
	printf("[007] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (2 !== ($num = mysql_num_fields($res)))
	printf("[008] Expecting 2 fields from 'test', got %d. [%d] %s\n", $num, mysql_errno($link), mysql_error($link));

var_dump(mysql_fetch_assoc($res));
for ($field_offset = 0; $field_offset < mysql_num_fields($res); $field_offset++) {
	printf("Field Offset %d\n", $field_offset);
	printf("mysql_field_flags(): %s\n", mysql_field_flags($res, $field_offset));
	printf("mysql_field_len(): %s\n", mysql_field_len($res, $field_offset));
	printf("mysql_field_name(): %s\n", mysql_field_name($res, $field_offset));
	printf("mysql_field_type(): %s\n", mysql_field_type($res, $field_offset));
}

mysql_free_result($res);
mysql_close($link);

print "done!";
?>
--CLEAN--
<?php
require_once 'connect.inc';
if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
	printf("[c001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

if (!mysql_query("DROP TABLE IF EXISTS test", $link))
	printf("[c002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

if (!mysql_query("DROP TABLE IF EXISTS test2", $link))
	printf("[c002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysql_close($link);
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
[006] [%d] %s
bool(false)
Field Offset 0
mysql_field_flags()%s
mysql_field_len(): 11
mysql_field_name(): id
mysql_field_type(): int
Field Offset 1
mysql_field_flags()%s
mysql_field_len(): %s
mysql_field_name(): label
mysql_field_type(): string
done!
