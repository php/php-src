--TEST--
mysql_get_server_info()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

if (null !== ($tmp = @mysql_get_server_info(NULL)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require "table.inc";
if (!is_string($info = mysql_get_server_info($link)) || ('' === $info))
	printf("[003] Expecting string/any_non_empty, got %s/%s\n", gettype($info), $info);

$def_info = mysql_get_server_info();
if ($def_info !== $info) {
	printf("[004] Server info for the default link and the specified link differ, [%d] %s\n",
		mysql_errno(), mysql_error());

	var_dump($def_info);
	var_dump($info);
}

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && 
    (version_compare(PHP_VERSION, '6.9.9', '<=') == 1) &&
    !is_unicode($info)) {
	printf("[005] Expecting Unicode error message!\n");
	var_inspect($info);
}

if (NULL !== ($tmp = @mysql_get_server_info('too many', 'just too many')))
	printf("[006] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

print "done!";
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
done!
