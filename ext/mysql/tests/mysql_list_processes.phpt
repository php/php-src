--TEST--
mysql_list_processes()
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

if (NULL !== ($tmp = @mysql_list_processes($link, $link)))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');

if (!$res = mysql_list_processes($link))
	printf("[002] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!$num = mysql_num_rows($res))
	printf("[003] Empty process list? [%d] %s\n", mysql_errno($link), mysql_error($link));

$row = mysql_fetch_array($res, MYSQL_NUM);
if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) &&
    (version_compare(PHP_VERSION, '6.9.9', '<=') == 1) &&
    !is_unicode($row[0])) {
	printf("[004] Check for unicode support\n");
	var_inspect($row);
}

mysql_free_result($res);

if (!$res = mysql_list_processes())
	printf("[005] [%d] %s\n", mysql_errno(), mysql_error());

if (!$num = mysql_num_rows($res))
	printf("[006] Empty process list? [%d] %s\n", mysql_errno(), mysql_error());

$row = mysql_fetch_array($res, MYSQL_NUM);
if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) &&
    (version_compare(PHP_VERSION, '6.9.9', '<=') == 1) &&
    !is_unicode($row[0])) {
	printf("[007] Check for unicode support\n");
	var_inspect($row);
}

mysql_free_result($res);
mysql_close($link);

print "done!\n";
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
done!
