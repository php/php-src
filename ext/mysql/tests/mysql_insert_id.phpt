--TEST--
mysql_insert_id()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('skipifdefaultconnectfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$tmp    = NULL;
$link   = NULL;

if (0 !== ($tmp = @mysql_insert_id()))
	printf("[001] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_insert_id($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!is_null($tmp = @mysql_insert_id($link, 'too many args')))
	printf("[002a] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');

if (0 !== ($tmp = mysql_insert_id($link)))
	printf("[003] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

if (!$res = mysql_query("SELECT id, label FROM test ORDER BY id LIMIT 1", $link)) {
	printf("[004] [%d] %s\n", mysql_errno($link), mysql_error($link));
}
if (0 !== ($tmp = mysql_insert_id($link)))
	printf("[005] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);
mysql_free_result($res);

// no auto_increment column
if (!$res = mysql_query("INSERT INTO test(id, label) VALUES (100, 'a')", $link)) {
	printf("[006] [%d] %s\n", mysql_errno($link), mysql_error($link));
}
if (0 !== ($tmp = mysql_insert_id($link)))
	printf("[007] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

if (!$res = mysql_query("ALTER TABLE test MODIFY id INT NOT NULL AUTO_INCREMENT", $link)) {
	printf("[008] [%d] %s\n", mysql_errno($link), mysql_error($link));
}
if (!$res = mysql_query("INSERT INTO test(label) VALUES ('a')", $link)) {
	printf("[009] [%d] %s\n", mysql_errno($link), mysql_error($link));
}
if (0 === ($tmp = mysql_insert_id($link)))
	printf("[010] Expecting int/not zero, got %s/%s\n", gettype($tmp), $tmp);

if ($tmp !== ($tmp2 = mysql_insert_id()))
	printf("[011] Expecting %s/%s, got %s/%s\n",
		gettype($tmp), $tmp,
		gettype($tmp2), $tmp2);

mysql_close($link);

var_dump(mysql_insert_id($link));

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysql_insert_id(): %d is not a valid MySQL-Link resource in %s on line %d
bool(false)
done!
