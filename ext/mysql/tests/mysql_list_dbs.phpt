--TEST--
mysql_list_dbs()
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

if (NULL !== ($tmp = @mysql_list_dbs(NULL)))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_list_dbs($link, $link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');

if (!$res = @mysql_list_dbs($link))
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!$num = mysql_num_rows($res))
	printf("[004] Empty database list? [%d] %s\n", mysql_errno($link), mysql_error($link));

$row = mysql_fetch_array($res, MYSQL_NUM);
if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && !is_unicode($row[0])) {
	printf("[005] Check for unicode support\n");
	var_inspect($row);
}

mysql_free_result($res);

if (!$res2 = @mysql_list_dbs())
	printf("[006] [%d] %s\n", mysql_errno(), mysql_error());

$row2 = mysql_fetch_array($res2, MYSQL_NUM);
mysql_free_result($res2);

assert($row === $row2);

mysql_close($link);

print "done!\n";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
done!