--TEST--
mysql_real_escape_string()
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
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
%unicode|string%(31) "Am I a unicode string in PHP 6?"
%unicode|string%(2) "\\"
%unicode|string%(2) "\""
%unicode|string%(2) "\'"
%unicode|string%(2) "\n"
%unicode|string%(2) "\r"
%unicode|string%(8) "foo\0bar"
done!
