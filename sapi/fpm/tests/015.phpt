--TEST--
FPM: Test various messages on start, from master and childs
--SKIPIF--
<?php include "skipif.inc"; ?>
--XFAIL--
randomly intermittently failing all the time in CI,
ERROR: unable to read what child say: Bad file descriptor (9)
catch_workers_output = yes seems not reliable
--FILE--
<?php

include "include.inc";

$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
$port1 = 9000+PHP_INT_SIZE;
$port2 = 9001+PHP_INT_SIZE;

$cfg = <<<EOT
[global]
error_log = $logfile
log_level = notice
[pool1]
listen = 127.0.0.1:$port1
listen.allowed_clients=127.0.0.1
user = foo
pm = dynamic
pm.max_children = 5
pm.min_spare_servers = 1
pm.max_spare_servers = 3
catch_workers_output = yes
[pool2]
listen = 127.0.0.1:$port2
listen.allowed_clients=xxx
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
catch_workers_output = yes
EOT;

$fpm = run_fpm($cfg, $tail);
if (is_resource($fpm)) {
    $i = 0;
	while (($i++ < 30) && !($fp = @fsockopen('127.0.0.1', $port1))) {
		usleep(10000);
	}
	if ($fp) {
		echo "Started\n";
		fclose($fp);
	}
	for ($i=0 ; $i<10 ; $i++) {
		try {
			run_request('127.0.0.1', $port1);
		} catch (Exception $e) {
			echo "Error 1\n";
		}
	}
	try {
		run_request('127.0.0.1', $port2);
	} catch (Exception $e) {
		echo "Error 2\n";
	}
	proc_terminate($fpm);
	if (!feof($tail)) {
		echo stream_get_contents($tail);
	}
	fclose($tail);
	proc_close($fpm);
}

?>
Done
--EXPECTF--
Started
Error 2
[%s] NOTICE: [pool pool1] pm.start_servers is not set. It's been set to 2.
[%s] NOTICE: [pool pool1] 'user' directive is ignored when FPM is not running as root
[%s] NOTICE: fpm is running, pid %d
[%s] NOTICE: ready to handle connections
[%s] WARNING: [pool pool2] child %d said into stderr: "ERROR: Wrong IP address 'xxx' in listen.allowed_clients"
[%s] WARNING: [pool pool2] child %d said into stderr: "ERROR: There are no allowed addresses for this pool"
[%s] WARNING: [pool pool2] child %d said into stderr: "ERROR: Connection disallowed: IP address '127.0.0.1' has been dropped."
[%s] NOTICE: Terminating ...
[%s] NOTICE: exiting, bye-bye!
Done
--CLEAN--
<?php
	$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
	@unlink($logfile);
?>