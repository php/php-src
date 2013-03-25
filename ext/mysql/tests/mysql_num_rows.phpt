--TEST--
mysql_num_rows()
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

if (!is_null($tmp = @mysql_num_rows()))
printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_num_rows($link)))
printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');

function func_test_mysql_num_rows($link, $query, $expected, $offset, $test_free = false) {

	if (!$res = mysql_query($query, $link)) {
		printf("[%03d] [%d] %s\n", $offset, mysql_errno($link), mysql_error($link));
		return;
	}

	if ($expected !== ($tmp = mysql_num_rows($res)))
		printf("[%03d] Expecting %s/%d, got %s/%d\n", $offset + 1,
			gettype($expected), $expected,
			gettype($tmp), $tmp);

	mysql_free_result($res);

	if ($test_free && (false !== ($tmp = mysql_num_rows($res))))
		printf("[%03d] Expecting boolean/false, got %s/%s\n", $offset + 2, gettype($tmp), $tmp);
}

func_test_mysql_num_rows($link, "SELECT 1 AS a", 1, 5);
func_test_mysql_num_rows($link, "SHOW VARIABLES LIKE '%nixnutz%'", 0, 10);
func_test_mysql_num_rows($link, "INSERT INTO test(id, label) VALUES (100, 'z')", NULL, 15);
func_test_mysql_num_rows($link, "SELECT id FROM test LIMIT 2", 2, 20, true);

if ($res = mysql_query('SELECT COUNT(id) AS num FROM test', $link)) {

	$row = mysql_fetch_assoc($res);
	mysql_free_result($res);

	func_test_mysql_num_rows($link, "SELECT id, label FROM test", (int)$row['num'], 25);

} else {
	printf("[030] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

if ($res = mysql_unbuffered_query('SELECT id, label FROM test')) {

	if (0 != mysql_num_rows($res))
		printf("[032] Expecting 0 rows got %d\n", mysql_num_rows($res));
	
	$rows = 0;
	while ($row = mysql_fetch_assoc($res))
		$rows++;

	if ($rows != mysql_num_rows($res))
		printf("[033] Expecting %d rows got %d\n", $rows, mysql_num_rows($res));

	mysql_free_result($res);
} else {
	printf("[034] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

mysql_close($link);
print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Warning: mysql_num_rows() expects parameter 1 to be resource, boolean given in %s on line %d

Warning: mysql_free_result() expects parameter 1 to be resource, boolean given in %s on line %d

Warning: mysql_num_rows(): %d is not a valid MySQL result resource in %s on line %d
done!
