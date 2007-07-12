--TEST--
mysql_real_escape_string()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "connect.inc";

$tmp    = NULL;
$link   = NULL;

if (NULL !== ($tmp = @mysql_real_escape_string()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');

if (NULL !== ($tmp = @mysql_real_escape_string('foo', $link, $link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

var_dump(mysql_real_escape_string("Am I a unicode string in PHP 6?", $link));
var_dump(mysql_real_escape_string('\\', $link));
var_dump(mysql_real_escape_string('"', $link));
var_dump(mysql_real_escape_string("'", $link));
var_dump(mysql_real_escape_string("\n", $link));
var_dump(mysql_real_escape_string("\r", $link));
var_dump($tmp = mysql_real_escape_string("foo" . chr(0) . "bar", $link));

assert($tmp === mysql_real_escape_string("foo" . chr(0) . "bar"));

print "done!";
?>
--EXPECTF--
string(31) "Am I a unicode string in PHP 6?"
string(2) "\\"
string(2) "\""
string(2) "\'"
string(2) "\n"
string(2) "\r"
string(8) "foo\0bar"
done!
--UEXPECTF--
unicode(31) "Am I a unicode string in PHP 6?"
unicode(2) "\\"
unicode(2) "\""
unicode(2) "\'"
unicode(2) "\n"
unicode(2) "\r"
unicode(8) "foo\0bar"
done!