--TEST--
mysql.trace_mode=1
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysql.trace_mode=1
error_reporting=E_ALL | E_NOTICE | E_STRICT
--FILE--
<?php
require_once('table.inc');

$res1 = mysql_query('SELECT id FROM test', $link);

if (!$res2 = @mysql_db_query($db, 'SELECT id FROM test', $link))
	printf("[001] [%d] %s\n", mysql_errno($link), mysql_error($link));
mysql_free_result($res2);
print @mysql_escape_string("I don't mind character sets, do I?\n");

$res3 = mysql_query('BOGUS_SQL', $link);
mysql_close($link);

print "done!\n";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
I don\'t mind character sets, do I?\n
Warning: mysql_query(): You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near 'BOGUS_SQL' at line 1 in %s on line %d
done!

Warning: Unknown: 1 result set(s) not freed. Use mysql_free_result to free result sets which were requested using mysql_query() in %s on line %d
