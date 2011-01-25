--TEST--
Check if deprecated API calls bail out
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
/*
  We use an extra test to cover deprecation warning.
  Due to this extra test we can silence deprecation warnings
  in have other test using @ operator without loosing the information
  which function is deprecated and, without reducing test portability.
*/
include "table.inc";

if (version_compare(PHP_VERSION, '5.3.0') >= 0) {
	$error = NULL;
	ob_start();
	if (!$res = mysql_db_query($db, "SELECT * FROM test", $link))
		$error .= sprintf("[001] [%d] %s\n", mysql_errno($link), mysql_error($link));
	else
	  mysql_free_result($res);
	$output = ob_get_contents();
	ob_end_clean();

	if (!stristr($output, 'deprecated')) {
		printf("[002] mysql_db_query has been deprecated in 5.3.0\n");
	}

	/*
		Deprecated since 2002 or the like but documented to be deprecated since 5.3.
		In 5.3 and before the deprecation message was bound to mysql.trace_mode=1.
		In 5.3.99 the warning will always be thrown, independent of the mysql.trace_mode
		setting.
	*/
	$error = NULL;
	ob_start();
	if (!$query = mysql_escape_string("charsets will be ignored"))
		$error .= sprintf("[005] [%d] %s\n", mysql_errno($link), mysql_error($link));
	$output = ob_get_contents();
	ob_end_clean();

	if (!stristr($output, 'deprecated')) {
		printf("[006] mysql_escape_string has been deprecated in 5.3.0\n");
	}

}

if (version_compare(PHP_VERSION, '5.3.99') >= 0) {
	$error = NULL;
	ob_start();
	if (!$res = mysql_list_dbs($link))
		$error .= sprintf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
	else
	  mysql_free_result($res);
	$output = ob_get_contents();
	ob_end_clean();

  if (!stristr($output, 'deprecated')) {
	printf("[004] mysql_db_query has been deprecated in 5.3.0\n");
  }
}



print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
done!