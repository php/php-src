--TEST--
FPM: Test IPv6 all addresses and access_log (bug #68421)
--SKIPIF--
<?php include "skipif.inc";
      @stream_socket_client('tcp://[::1]:0', $errno);
      if ($errno != 111) die('skip IPv6 not supported.');
?>
--FILE--
<?php

include "include.inc";

$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
$accfile = dirname(__FILE__).'/php-fpm.acc.tmp';
$port = 9000+PHP_INT_SIZE;

$cfg = <<<EOT
[global]
error_log = $logfile
[unconfined]
listen = [::]:$port
access.log = $accfile
ping.path = /ping
ping.response = pong
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$fpm = run_fpm($cfg, $tail);
if (is_resource($fpm)) {
    fpm_display_log($tail, 2);
    try {
		var_dump(strpos(run_request('127.0.0.1', $port), 'pong'));
		echo "IPv4 ok\n";
	} catch (Exception $e) {
		echo "IPv4 error\n";
	}
    try {
		var_dump(strpos(run_request('[::1]', $port), 'pong'));
		echo "IPv6 ok\n";
	} catch (Exception $e) {
		echo "IPv6 error\n";
	}
    proc_terminate($fpm);
    stream_get_contents($tail);
    fclose($tail);
    proc_close($fpm);

    echo file_get_contents($accfile);
}

?>
--EXPECTF--
[%d-%s-%d %d:%d:%d] NOTICE: fpm is running, pid %d
[%d-%s-%d %d:%d:%d] NOTICE: ready to handle connections
int(%d)
IPv4 ok
int(%d)
IPv6 ok
127.0.0.1 %s "GET /ping" 200
::1 %s "GET /ping" 200
--CLEAN--
<?php
    $logfile = dirname(__FILE__).'/php-fpm.log.tmp';
    @unlink($logfile);
	$accfile = dirname(__FILE__).'/php-fpm.acc.tmp';
    @unlink($accfile);
?>
