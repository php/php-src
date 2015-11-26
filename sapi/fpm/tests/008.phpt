--TEST--
FPM: Test multi pool (dynamic + ondemand + static) (bug #68423)
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
$port1 = 9000+PHP_INT_SIZE;
$port2 = 9001+PHP_INT_SIZE;
$port3 = 9002+PHP_INT_SIZE;

$cfg = <<<EOT
[global]
error_log = $logfile
[pool_dynamic]
listen = 127.0.0.1:$port1
ping.path = /ping
ping.response = pong-dynamic
pm = dynamic
pm.max_children = 5
pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
[poold_ondemand]
listen = 127.0.0.1:$port2
ping.path = /ping
ping.response = pong-on-demand
pm = ondemand
pm.max_children = 2
pm.process_idle_timeout = 10
[pool_static]
listen = 127.0.0.1:$port3
ping.path = /ping
ping.response = pong-static
pm = static
pm.max_children = 2
EOT;

$fpm = run_fpm($cfg, $tail);
if (is_resource($fpm)) {
    fpm_display_log($tail, 2);
    try {
		var_dump(strpos(run_request('127.0.0.1', $port1), 'pong-dynamic'));
		echo "Dynamic ok\n";
	} catch (Exception $e) {
		echo "Dynamic error\n";
	}
    try {
		var_dump(strpos(run_request('127.0.0.1', $port2), 'pong-on-demand'));
		echo "OnDemand ok\n";
	} catch (Exception $e) {
		echo "OnDemand error\n";
	}
    try {
		var_dump(strpos(run_request('127.0.0.1', $port3), 'pong-static'));
		echo "Static ok\n";
	} catch (Exception $e) {
		echo "Static error\n";
	}

	proc_terminate($fpm);
    stream_get_contents($tail);
    fclose($tail);
    proc_close($fpm);
}

?>
--EXPECTF--
[%d-%s-%d %d:%d:%d] NOTICE: fpm is running, pid %d
[%d-%s-%d %d:%d:%d] NOTICE: ready to handle connections
int(%d)
Dynamic ok
int(%d)
OnDemand ok
int(%d)
Static ok
--CLEAN--
<?php
    $logfile = dirname(__FILE__).'/php-fpm.log.tmp';
    @unlink($logfile);
?>
