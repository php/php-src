--TEST--
FPM: Test pool prefix
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$prefix = __DIR__;
$logfile = __DIR__.'/php-fpm.log.tmp';
$accfile = 'php-fpm.acc.tmp';
$slwfile = 'php-fpm.slw.tmp';
$pidfile = __DIR__.'/php-fpm.pid.tmp';
$port = 9000+PHP_INT_SIZE;
$cfg = <<<EOT

[global]
error_log = $logfile
pid = $pidfile
[test]
prefix = $prefix;
listen = 127.0.0.1:$port
access.log = $accfile
slowlog = $slwfile;
request_slowlog_timeout = 1
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
		run_request('127.0.0.1', $port);
		echo "Ping ok\n";
	} catch (Exception $e) {
		echo "Ping error\n";
	}
	printf("File %s %s\n", $accfile, (file_exists(__DIR__.'/'.$accfile) ? "exists" : "missing"));
	printf("File %s %s\n", $slwfile, (file_exists(__DIR__.'/'.$slwfile) ? "exists" : "missing"));

	proc_terminate($fpm);
	echo stream_get_contents($tail);
    fclose($tail);
    proc_close($fpm);
	readfile(__DIR__.'/'.$accfile);
}

?>
--EXPECTF--
[%s] NOTICE: fpm is running, pid %d
[%s] NOTICE: ready to handle connections
Ping ok
File php-fpm.acc.tmp exists
File php-fpm.slw.tmp exists
[%s] NOTICE: Terminating ...
[%s] NOTICE: exiting, bye-bye!
127.0.0.1 -  %s "GET /ping" 200
--CLEAN--
<?php
	$logfile = __DIR__.'/php-fpm.log.tmp';
	$accfile = __DIR__.'/php-fpm.acc.tmp';
	$slwfile = __DIR__.'/php-fpm.slw.tmp';
	$pidfile = __DIR__.'/php-fpm.pid.tmp';
    @unlink($logfile);
    @unlink($accfile);
    @unlink($slwfile);
    @unlink($pidfile);
?>
