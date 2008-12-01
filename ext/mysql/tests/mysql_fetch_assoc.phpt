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
--EXPECTF--
[005]
array(2) {
  ["id"]=>
  string(1) "1"
  ["label"]=>
  string(1) "a"
}
[006]
bool(false)
[008]
array(5) {
  ["a"]=>
  string(1) "2"
  ["c"]=>
  string(1) "3"
  ["C"]=>
  string(1) "4"
  ["d"]=>
  NULL
  ["e"]=>
  string(1) "1"
}

Warning: mysql_fetch_assoc(): %d is not a valid MySQL result resource in %s on line %d
[010]
array(5) {
  ["id"]=>
  string(1) "1"
  ["label"]=>
  string(1) "a"
  ["_id"]=>
  string(1) "1"
  ["_label"]=>
  string(2) "aa"
  ["_foo"]=>
  NULL
}
done!
--UEXPECTF--
[005]
array(2) {
  [u"id"]=>
  unicode(1) "1"
  [u"label"]=>
  unicode(1) "a"
}
[006]
bool(false)
[008]
array(5) {
  [u"a"]=>
  unicode(1) "2"
  [u"c"]=>
  unicode(1) "3"
  [u"C"]=>
  unicode(1) "4"
  [u"d"]=>
  NULL
  [u"e"]=>
  unicode(1) "1"
}

Warning: mysql_fetch_assoc(): %d is not a valid MySQL result resource in %s on line %d
[010]
array(5) {
  [u"id"]=>
  unicode(1) "1"
  [u"label"]=>
  unicode(1) "a"
  [u"_id"]=>
  unicode(1) "1"
  [u"_label"]=>
  unicode(2) "aa"
  [u"_foo"]=>
  NULL
}
done!
