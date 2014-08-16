--TEST--
mysql_fetch_assoc()
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

// Note: no SQL type tests, internally the same function gets used as for mysql_fetch_array() which does a lot of SQL type test

if (!is_null($tmp = @mysql_fetch_assoc()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_fetch_assoc($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');
if (!$res = mysql_query("SELECT id, label FROM test ORDER BY id LIMIT 1", $link)) {
	printf("[004] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

print "[005]\n";
var_dump(mysql_fetch_assoc($res));

print "[006]\n";
var_dump(mysql_fetch_assoc($res));

mysql_free_result($res);

if (!$res = mysql_query("SELECT 1 AS a, 2 AS a, 3 AS c, 4 AS C, NULL AS d, true AS e", $link)) {
	printf("[007] Cannot run query, [%d] %s\n", mysql_errno($link), $mysql_error($link));
}
print "[008]\n";
var_dump(mysql_fetch_assoc($res));

mysql_free_result($res);

if (false !== ($tmp = mysql_fetch_assoc($res)))
	printf("[008] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

mysql_close($link);

include('table.inc');
if (!$res = mysql_query("SELECT id, label, id AS _id, CONCAT(label, 'a') _label, NULL as _foo FROM test _test ORDER BY id ASC LIMIT 1", $link)) {
	printf("[009] [%d] %s\n", mysql_errno($link), $mysql_error($link));
}
print "[010]\n";
var_dump(mysql_fetch_assoc($res));
mysql_free_result($res);

mysql_close($link);

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
[005]
array(2) {
  [%u|b%"id"]=>
  %unicode|string%(1) "1"
  [%u|b%"label"]=>
  %unicode|string%(1) "a"
}
[006]
bool(false)
[008]
array(5) {
  [%u|b%"a"]=>
  %unicode|string%(1) "2"
  [%u|b%"c"]=>
  %unicode|string%(1) "3"
  [%u|b%"C"]=>
  %unicode|string%(1) "4"
  [%u|b%"d"]=>
  NULL
  [%u|b%"e"]=>
  %unicode|string%(1) "1"
}

Warning: mysql_fetch_assoc(): supplied resource is not a valid MySQL result resource in %s on line %d

Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
[010]
array(5) {
  [%u|b%"id"]=>
  %unicode|string%(1) "1"
  [%u|b%"label"]=>
  %unicode|string%(1) "a"
  [%u|b%"_id"]=>
  %unicode|string%(1) "1"
  [%u|b%"_label"]=>
  %unicode|string%(2) "aa"
  [%u|b%"_foo"]=>
  NULL
}
done!
