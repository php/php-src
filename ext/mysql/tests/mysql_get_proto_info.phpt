--TEST--
mysql_get_proto_info()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include_once "connect.inc";

if (NULL !== ($tmp = @mysql_get_proto_info(NULL)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require "table.inc";
if (!is_int($info = mysql_get_proto_info($link)) || (0 === $info))
	printf("[003] Expecting int/any_non_empty, got %s/%s\n", gettype($info), $info);

if (!is_int($info2 = mysql_get_proto_info()) || (0 === $info2))
	printf("[004] Expecting int/any_non_empty, got %s/%s\n", gettype($info2), $info2);

assert($info === $info2);

if (NULL !== ($tmp = @mysql_get_proto_info('too many', 'arguments')))
	printf("[005] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
done!
