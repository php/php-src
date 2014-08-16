--TEST--
mysql_result()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once("connect.inc");

$tmp    = NULL;
$link   = NULL;

// string mysql_result ( resource result, int row [, mixed field] )

if (!is_null($tmp = @mysql_result()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!is_null($tmp = @mysql_result($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require_once('table.inc');
if (!$res = mysql_query("SELECT id, label, id AS _id, CONCAT(label, 'a') _label, NULL as _foo FROM test _test ORDER BY id ASC LIMIT 1", $link)) {
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

var_dump(mysql_result($res, -1));
var_dump(mysql_result($res, 2));

var_dump(mysql_result($res, 0, -1));
var_dump(mysql_result($res, 0, 500));

print "valid fields\n";
var_dump(mysql_result($res, 0));
var_dump(mysql_result($res, 0, 1));

var_dump(mysql_result($res, 0, 'id'));
var_dump(mysql_result($res, 0, '_test.id'));
var_dump(mysql_result($res, 0, 'label'));
var_dump(mysql_result($res, 0, '_test.label'));
print "some invalid fields\n";
var_dump(mysql_result($res, 0, 'unknown'));
var_dump(mysql_result($res, 0, '_test.'));
var_dump(mysql_result($res, 0, chr(0)));
var_dump(mysql_result($res, 0, '_test.' . chr(0)));
print "_id\n";
var_dump(mysql_result($res, 0, '_id'));
print "_label\n";
var_dump(mysql_result($res, 0, '_label'));
print "_foo\n";
var_dump(mysql_result($res, 0, '_foo'));
var_dump(mysql_result($res, 0, 'test.id'));
var_dump(mysql_result($res, 0, 'test.label'));

mysql_free_result($res);

var_dump(mysql_result($res, 0));

mysql_close($link);
print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Warning: mysql_result(): Unable to jump to row -1 on MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): Unable to jump to row 2 on MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): Bad column offset specified in %s on line %d
bool(false)

Warning: mysql_result(): Bad column offset specified in %s on line %d
bool(false)
valid fields
%unicode|string%(1) "1"
%unicode|string%(1) "a"
%unicode|string%(1) "1"
%unicode|string%(1) "1"
%unicode|string%(1) "a"
%unicode|string%(1) "a"
some invalid fields

Warning: mysql_result(): unknown not found in MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): _test. not found in MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result():  not found in MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): _test. not found in MySQL result index %d in %s on line %d
bool(false)
_id
%unicode|string%(1) "1"
_label
string(2) "aa"
_foo
NULL

Warning: mysql_result(): test.id not found in MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): test.label not found in MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): supplied resource is not a valid MySQL result resource in %s on line %d
bool(false)
done!
