--TEST--
phpinfo() mysqli section
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include("connect.inc");

	@ob_clean();
	ob_start();
	phpinfo();
	$phpinfo = ob_get_contents();
	ob_end_clean();

	/* all versions should at least dump this minimum information */
	if (!stristr($phpinfo, "mysqli support"))
		printf("[001] ext/mysqli should have exposed itself.\n");

	if (!stristr($phpinfo, "client api library version"))
		printf("[002] ext/mysqli should have exposed the library version.\n");

	if (!stristr($phpinfo, "mysqli.default_host"))
		printf("[003] php.ini setting mysqli.default_host not shown.\n");

	if (!stristr($phpinfo, "mysqli.default_port"))
		printf("[004] php.ini setting mysqli.default_port not shown.\n");

	if (!stristr($phpinfo, "mysqli.default_pw"))
		printf("[005] php.ini setting mysqli.default_pw not shown.\n");

	if (!stristr($phpinfo, "mysqli.default_socket"))
		printf("[006] php.ini setting mysqli.default_socket not shown.\n");

	if (!stristr($phpinfo, "mysqli.default_user"))
		printf("[007] php.ini setting mysqli.default_user not shown.\n");

	if (!stristr($phpinfo, "mysqli.max_links"))
		printf("[008] php.ini setting mysqli.max_links not shown.\n");

	if (!stristr($phpinfo, "mysqli.reconnect"))
		printf("[009] php.ini setting mysqli.reconnect not shown.\n");

	if ($IS_MYSQLND) {
		$expected = array(
			'mysqlnd statistics',
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
			'mysqli.allow_local_infile',
			'mysqli.allow_persistent', 'mysqli.max_persistent'
		);
		foreach ($expected as $k => $entry)
			if (!stristr($phpinfo, $entry))
				printf("[010] Could not find entry for '%s'\n", $entry);
	}

	print "done!";
?>
--EXPECT--
done!
