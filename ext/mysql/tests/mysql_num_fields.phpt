--TEST--
mysql_num_fields()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$tmp    = NULL;
$link   = NULL;

if (!is_null($tmp = @mysql_num_fields()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_num_fields($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');

function func_test_mysql_num_fields($link, $query, $expected, $offset, $test_free = false) {

if (!($res = mysql_query($query, $link))) {
	printf("[%03d] [%d] %s\n", $offset, mysql_errno($link), mysql_error($link));
	return;
}

if ($expected !== ($tmp = mysql_num_fields($res)))
	printf("[%03d] Expecting %s/%d, got %s/%d\n", $offset + 1,
	gettype($expected), $expected,
	gettype($tmp), $tmp);

mysql_free_result($res);

if ($test_free && (false !== ($tmp = mysql_num_fields($res))))
	printf("[%03d] Expecting boolean/false, got %s/%s\n", $offset + 2, gettype($tmp), $tmp);
}

func_test_mysql_num_fields($link, "SELECT 1 AS a", 1, 5);
func_test_mysql_num_fields($link, "SELECT id, label FROM test", 2, 10);
func_test_mysql_num_fields($link, "SELECT 1 AS a, NULL AS b, 'foo' AS c", 3, 15);
func_test_mysql_num_fields($link, "SELECT id FROM test", 1, 20, true);

mysql_close($link);

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysql_num_fields(): %d is not a valid MySQL result resource in %s on line %d
done!
