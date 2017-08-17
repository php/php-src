--TEST--
FPM: Test splited configuration and load order #68391
--SKIPIF--
<?php
include "skipif.inc";

$cfg = <<<EOT
[global]
error_log = /dev/null
[poold_ondemand]
listen=127.0.0.1:9000
pm = ondemand
pm.max_children = 2
pm.process_idle_timeout = 10
EOT;

if (test_fpm_conf($cfg, $msg) == false) {
	die("skip " .  $msg);
}
?>
--FILE--
<?php

include "include.inc";

$logfile = __DIR__.'/php-fpm.log.tmp';
$logdir  = __DIR__.'/conf.d';
$port = 9000+PHP_INT_SIZE;

// Main configuration
$cfg = <<<EOT
[global]
error_log = $logfile
log_level = notice
include = $logdir/*.conf
EOT;

// Splited configuration
@mkdir($logdir);
$i=$port;
$names = ['cccc', 'aaaa', 'eeee', 'dddd', 'bbbb'];
foreach($names as $name) {
	$poolcfg = <<<EOT
[$name]
listen = 127.0.0.1:$i
listen.allowed_clients=127.0.0.1
user = foo
pm = ondemand
pm.max_children = 5
EOT;
	file_put_contents("$logdir/$name.conf", $poolcfg);
	$i++;
}

// Test
$fpm = run_fpm($cfg, $tail);
if (is_resource($fpm)) {
    fpm_display_log($tail, count($names)+2);
	$i=$port;
	foreach($names as $name) {
		try {
			run_request('127.0.0.1', $i++);
			echo "OK $name\n";
		} catch (Exception $e) {
			echo "Error 1\n";
		}
	}
	proc_terminate($fpm);
	fpm_display_log($tail, -1);
	fclose($tail);
	proc_close($fpm);
}

?>
Done
--EXPECTF--
[%s] NOTICE: [pool aaaa] 'user' directive is ignored when FPM is not running as root
[%s] NOTICE: [pool bbbb] 'user' directive is ignored when FPM is not running as root
[%s] NOTICE: [pool cccc] 'user' directive is ignored when FPM is not running as root
[%s] NOTICE: [pool dddd] 'user' directive is ignored when FPM is not running as root
[%s] NOTICE: [pool eeee] 'user' directive is ignored when FPM is not running as root
[%s] NOTICE: fpm is running, pid %d
[%s] NOTICE: ready to handle connections
OK cccc
OK aaaa
OK eeee
OK dddd
OK bbbb
[%s] NOTICE: Terminating ...
[%s] NOTICE: exiting, bye-bye!
Done
--CLEAN--
<?php
	$logfile = __DIR__.'/php-fpm.log.tmp';
	$logdir  = __DIR__.'/conf.d';
	@unlink($logfile);
	foreach(glob("$logdir/*.conf") as $name) {
		unlink($name);
	}
	@rmdir($logdir);
?>
