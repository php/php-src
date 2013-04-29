--TEST--
mysql_db_query()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

// NOTE: this function is deprecated. We do only the most necessary
// to test it. We don't test all aspects of the documented behaviour.

$tmp    = NULL;
$link   = NULL;

if (NULL !== ($tmp = @mysql_db_query()))
	printf("[001] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_db_query($link)))
	printf("[002] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_db_query($link)))
	printf("[003] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');
if (!$res = @mysql_db_query($db, 'SELECT id, label FROM test ORDER BY id LIMIT 1', $link))
	printf("[004] [%d] %s\n", mysql_errno($link), mysql_error($link));

$row = mysql_fetch_assoc($res);
if (1 != $row['id'])
	printf("[005] Expecting record 1/a, got record %s/%s\n", $row['id'], $row['label']);

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && !is_unicode($row['label'])) {
	printf("[006] No unicode returned! [%d] %s\n", mysql_errno($link), mysql_error($link));
	var_inspect($row);
}

mysql_free_result($res);


if (!$res = @mysql_db_query($db, 'SELECT id, label FROM test ORDER BY id LIMIT 1'))
	printf("[007] [%d] %s\n", mysql_errno(), mysql_error());

$row = mysql_fetch_assoc($res);
if (1 != $row['id'])
	printf("[008] Expecting record 1/a, got record %s/%s\n", $row['id'], $row['label']);

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && !is_unicode($row['label'])) {
	printf("[009] No unicode returned! [%d] %s\n", mysql_errno(), mysql_error());
	var_inspect($row);
}

mysql_free_result($res);
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
