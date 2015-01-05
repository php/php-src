--TEST--
mysql_fetch_lengths()
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

if (!is_null($tmp = @mysql_fetch_lengths()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_fetch_lengths($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');
if (!$res = mysql_query("SELECT id, label FROM test ORDER BY id LIMIT 1", $link)) {
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

while ($row = mysql_fetch_assoc($res))
	var_dump(mysql_fetch_lengths($res));
var_dump(mysql_fetch_lengths($res));

mysql_free_result($res);

var_dump(mysql_fetch_lengths($res));

mysql_close($link);
print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}
bool(false)

Warning: mysql_fetch_lengths(): supplied resource is not a valid MySQL result resource in %s on line %d
bool(false)
done!
