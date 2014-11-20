--TEST--
FPM: Test various messages on start, from master and childs
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
$port = 9000+PHP_INT_SIZE;

$cfg = <<<EOT
[global]
error_log = $logfile
log_level = notice
[unconfined]
listen = 127.0.0.1:$port
listen.allowed_clients=127.0.0.1,xxx
user = foo
pm = dynamic
pm.max_children = 5
;pm.start_servers = 2
pm.min_spare_servers = 1
pm.max_spare_servers = 3
catch_workers_output = yes
EOT;

$fpm = run_fpm($cfg, $tail);
if (is_resource($fpm)) {
    $i = 0;
	while (($i++ < 30) && !($fp = @fsockopen('127.0.0.1', $port))) {
		usleep(10000);
	}
	if ($fp) {
		echo "Started\n";
		fclose($fp);
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
[%s] NOTICE: [pool unconfined] pm.start_servers is not set. It's been set to 2.
[%s] NOTICE: [pool unconfined] 'user' directive is ignored when FPM is not running as root
[%s] NOTICE: fpm is running, pid %d
[%s] NOTICE: ready to handle connections
[%s] WARNING: [pool unconfined] child %d said into stderr: "ERROR: Wrong IP address 'xxx' in listen.allowed_clients"
[%s] WARNING: [pool unconfined] child %d said into stderr: "ERROR: Wrong IP address 'xxx' in listen.allowed_clients"
[%s] NOTICE: Terminating ...
[%s] NOTICE: exiting, bye-bye!
Done
--CLEAN--
<?php
	$logfile = dirname(__FILE__).'/php-fpm.log.tmp';
	@unlink($logfile);
?>