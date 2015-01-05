--TEST--
mysql_stat()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include_once "connect.inc";

$dbname = 'test';
$tmp    = NULL;
$link   = NULL;

if (!is_null($tmp = @mysql_stat($link)))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');

if (!is_null($tmp = @mysql_stat($link, "foo")))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if ((!is_string($stat = mysql_stat($link))) || ('' === $stat))
	printf("[003] Expecting non empty string, got %s/'%s', [%d] %s\n",
		gettype($stat), $stat, mysql_errno($link), mysql_error($link));

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) &&
    (version_compare(PHP_VERSION, '6.9.9', '<=') == 1) &&
    !is_unicode($stat)) {
	printf("[004] Expecting Unicode error message!\n");
	var_inspect($stat);
}

if ((!is_string($stat_def = mysql_stat())) || ('' === $stat_def))
	printf("[003] Expecting non empty string, got %s/'%s', [%d] %s\n",
		gettype($stat_def), $stat_def, mysql_errno(), mysql_error());

assert(soundex($stat) === soundex($stat_def));

mysql_close($link);

if (false !== ($tmp = mysql_stat($link)))
	printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

print "done!";
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Warning: mysql_stat(): supplied resource is not a valid MySQL-Link resource in %s on line %d
done!
