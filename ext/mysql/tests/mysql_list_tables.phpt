--TEST--
mysql_list_tables()
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

if (NULL !== ($tmp = @mysql_list_tables()))
	printf("[001] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_list_tables('too', 'many', 'arguments')))
	printf("[002] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

if (false !== ($tmp = @mysql_list_tables(NULL)))
	printf("[003] Expecting boolean/false got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_list_tables($db, NULL)))
	printf("[004] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

require_once('table.inc');

if (!$res_def = @mysql_list_tables($db))
	printf("[005] [%d] %s\n", mysql_errno(), mysql_error());

if (!$res = @mysql_list_tables($db, $link))
	printf("[006] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!$res_query = mysql_query("SHOW TABLES", $link))
	printf("[007] [%d] %s\n", mysql_errno($link), mysql_error($link));

$tables_def = $tables = $tables_query = array();

while ($row = mysql_fetch_assoc($res_def))
	$tables_def[] = $row;
mysql_free_result($res_def);

while ($row = mysql_fetch_assoc($res))
	$tables[] = $row;
mysql_free_result($res);

while ($row = mysql_fetch_assoc($res_query))
	$tables_query[] = $row;
mysql_free_result($res_query);

if ($tables_def !== $tables) {
	printf("[008] Got different table lists for default link and specified link\n");
	var_dump($tables_def);
	var_dump($tables);
}

$list1 = $list2 = array();
foreach ($tables as $k => $tlist)
	foreach ($tlist as $k => $table)
		$list1[] = $table;

foreach ($tables_query as $k => $tlist)
	foreach ($tlist as $k => $table)
		$list2[] = $table;

if ($list1 !== $list2) {
	printf("[009] Got different results for mysql_list_tables() and SHOW TABLES\n");
	var_dump($list1);
	var_dump($list2);
}

if (!in_array('test', $list1))
	printf("[010] Table lists seem to be wrong. Check manually.\n");

mysql_close($link);

print "done!\n";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
done!
