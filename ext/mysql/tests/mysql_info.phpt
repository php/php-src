--TEST--
mysql_info()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

if (false !== ($tmp = @mysql_info()))
	printf("[001] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_info(NULL)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require "table.inc";
if (!$res = mysql_query("INSERT INTO test(id, label) VALUES (100, 'a')", $link))
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (false !== ($tmp = mysql_info($link)))
	printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if (!$res = mysql_query("INSERT INTO test(id, label) VALUES (101, 'a'), (102, 'b')", $link))
	printf("[005] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!is_string($tmp = mysql_info($link)) || ('' == $tmp))
	printf("[006] Expecting string/any_non_empty, got %s/%s\n", gettype($tmp), $tmp);

if (!$res = mysql_query('INSERT INTO test(id, label) SELECT id + 200, label FROM test', $link))
	printf("[007] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!is_string($tmp = mysql_info($link)) || ('' == $tmp))
	printf("[008] Expecting string/any_non_empty, got %s/%s\n", gettype($tmp), $tmp);

if (!$res = mysql_query('ALTER TABLE test MODIFY label CHAR(2)', $link))
	printf("[009] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!is_string($tmp = mysql_info($link)) || ('' == $tmp))
	printf("[010] Expecting string/any_non_empty, got %s/%s\n", gettype($tmp), $tmp);

if (!$res = mysql_query("UPDATE test SET label = 'b' WHERE id >= 100", $link))
	printf("[011] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!is_string($tmp = mysql_info($link)) || ('' == $tmp))
	printf("[012] Expecting string/any_non_empty, got %s/%s\n", gettype($tmp), $tmp);

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) &&
    (version_compare(PHP_VERSION, '6.9.9', '<=') == 1) &&
    !is_unicode($tmp)) {
	printf("[013] Expecting Unicode!\n");
	var_inspect($info);
}

if (!is_string($def_tmp = mysql_info()) || ('' == $def_tmp))
	printf("[014] Expecting string/any_non_empty, got %s/%s\n", gettype($def_tmp), $def_tmp);

if ($def_tmp !== $tmp) {
	printf("[015] Results differ for default link and specified link, [%d] %s\n",
		mysql_errno(), mysql_error());
	var_inspect($tmp);
	var_inspect($def_tmp);
}

// NOTE: no LOAD DATA INFILE test

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
done!
