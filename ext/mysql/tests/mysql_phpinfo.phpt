--TEST--
phpinfo() mysql section
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include_once("connect.inc");

@ob_clean();
ob_start();
phpinfo();
$phpinfo = ob_get_contents();
ob_end_clean();

/* all versions should at least dump this minimum information */
if (!stristr($phpinfo, "mysql support"))
	printf("[001] ext/mysql should have exposed itself.\n");

if (!stristr($phpinfo, "client api version"))
	printf("[002] ext/mysql should have exposed the library version.\n");

if (!stristr($phpinfo, "mysql.default_host"))
	printf("[003] php.ini setting mysql.default_host not shown.\n");

if (!stristr($phpinfo, "mysql.default_port"))
	printf("[004] php.ini setting mysql.default_port not shown.\n");

if (!stristr($phpinfo, "mysql.default_password"))
	printf("[005] php.ini setting mysql.default_password not shown.\n");

if (!stristr($phpinfo, "mysql.default_socket"))
	printf("[006] php.ini setting mysql.default_socket not shown.\n");

if (!stristr($phpinfo, "mysql.default_user"))
	printf("[007] php.ini setting mysql.default_user not shown.\n");

if (!stristr($phpinfo, "mysql.max_links"))
	printf("[008] php.ini setting mysql.max_links not shown.\n");

if (!stristr($phpinfo, "mysql.max_persistent"))
	printf("[009] php.ini setting mysql.max_persistent not shown.\n");

if (!stristr($phpinfo, "mysql.connect_timeout"))
	printf("[010] php.ini setting mysql.connect_timeout not shown.\n");

if (!stristr($phpinfo, "mysql.allow_persistent"))
	printf("[011] php.ini setting mysql.allow_persistent not shown.\n");

if ($IS_MYSQLND) {
	$expected = array(
		'client statistics',
		'bytes_sent', 'bytes_received', 'packets_sent', 'packets_received',
		'protocol_overhead_in', 'protocol_overhead_out', 'result_set_queries',
		'non_result_set_queries', 'no_index_used', 'bad_index_used',
		'buffered_sets', 'unbuffered_sets', 'ps_buffered_sets', 'ps_unbuffered_sets',
		'flushed_normal_sets', 'flushed_ps_sets', 'rows_fetched_from_server',
		'rows_fetched_from_client', 'rows_skipped', 'copy_on_write_saved',
		'copy_on_write_performed', 'command_buffer_too_small', 'connect_success',
		'connect_failure', 'connection_reused', 'explicit_close', 'implicit_close',
		'disconnect_close', 'in_middle_of_command_close', 'explicit_free_result',
		'implicit_free_result', 'explicit_stmt_close', 'implicit_stmt_close',
		'size',
	);
	foreach ($expected as $k => $entry)
		if (!stristr($phpinfo, $entry))
			printf("[012] Could not find entry for '%s'\n", $entry);
}

print "done!";
?>
--EXPECTF--
done!
