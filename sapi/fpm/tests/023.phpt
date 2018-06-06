--TEST--
FPM: Test already bound address
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
$port = 9000+PHP_INT_SIZE;

$cfg = <<<EOT
[global]
log_level = debug
error_log = $logfile
[unconfined]
listen = $port
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

// Occupy our port and let things fail
$busy = stream_socket_server("tcp://[::]:$port");

$fpm = run_fpm($cfg, $tail);
if (is_resource($fpm)) {
	/* Expect two specific lines of log output and show them
	 * If we get any different number of those patterns, display whole log
	 */
	$out = $all = '';
	$count = 0;
	while (!feof($tail)) {
		$line = fgets($tail);
		$all .= $line;
		if ((false !== strpos($line, 'retrying with 0.0.0.0')) ||
		    (false !== strpos($line, 'unable to bind'))) {
			$out .= $line;
			++$count;
		}
	}
	echo ($count == 2) ? $out : $all;
}
?>
--EXPECTF--
[%d-%s-%d %d:%d:%f] NOTICE: pid %d, fpm_socket_af_inet_listening_socket(), line %d: Failed implicitly binding to ::, retrying with 0.0.0.0
[%d-%s-%d %d:%d:%f] ERROR: pid %d, fpm_sockets_new_listening_socket(), line %d: unable to bind listening socket for address '%d': %s
--CLEAN--
<?php
    $logfile = dirname(__FILE__).'/php-fpm.log.tmp';
    @unlink($logfile);
?>
