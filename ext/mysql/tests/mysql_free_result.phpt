--TEST--
mysql_free_result()
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

if (!is_null($tmp = @mysql_free_result()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (null !== ($tmp = @mysql_free_result($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');
if (!$res = mysql_query("SELECT id FROM test ORDER BY id LIMIT 1", $link)) {
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

var_dump(mysql_free_result($res));
var_dump(mysql_free_result($res));

if ($tmp = sys_get_temp_dir()) {
	$tmpname = tempnam($tmp, 'free_result');
	if ($fp = fopen($tmpname, 'w')) {
		if (false !== ($tmp = @mysql_free_result($fp)))
			printf("[004] Expecting boolean/false got %s/%s\n", gettype($tmp), $tmp);
		fclose($fp);
		@unlink($tmpname);
	}
}

mysql_close($link);
print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
bool(true)

Warning: mysql_free_result(): %d is not a valid MySQL result resource in %s on line %d
bool(false)
done!
