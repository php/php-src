--TEST--
FPM: Test fastcgi_get_status function
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$logfile = __DIR__.'/php-fpm.log.tmp';
$srcfile = __DIR__.'/php-fpm.tmp.php';
$port = 9000+PHP_INT_SIZE;

$cfg = <<<EOT
[global]
error_log = $logfile
[unconfined]
listen = 127.0.0.1:$port
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$code = <<<EOT
<?php
echo "Test Start\n";
var_dump(fastcgi_get_status());
echo "Test End\n";
EOT;
file_put_contents($srcfile, $code);

$fpm = run_fpm($cfg, $tail);
if (is_resource($fpm)) {
    fpm_display_log($tail, 2);
    try {
		$req = run_request('127.0.0.1', $port, $srcfile);
		echo strstr($req, "Test Start");
		echo "Request ok\n";
	} catch (Exception $e) {
		echo "Request error\n";
	}
    proc_terminate($fpm);
    fpm_display_log($tail, -1);
    fclose($tail);
    proc_close($fpm);
}

?>
Done
--EXPECTF--
[%s] NOTICE: fpm is running, pid %d
[%s] NOTICE: ready to handle connections
Test Start
array(15) {
  ["pool"]=>
  string(10) "unconfined"
  ["process-manager"]=>
  string(7) "dynamic"
  ["start-time"]=>
  int(%d)
  ["start-since"]=>
  int(%d)
  ["accepted-conn"]=>
  int(1)
  ["listen-queue"]=>
  int(0)
  ["max-listen-queue"]=>
  int(0)
  ["listen-queue-len"]=>
  int(128)
  ["idle-processes"]=>
  int(0)
  ["active-processes"]=>
  int(1)
  ["total-processes"]=>
  int(1)
  ["max-active-processes"]=>
  int(1)
  ["max-children-reached"]=>
  int(0)
  ["slow-requests"]=>
  int(0)
  ["procs"]=>
  array(1) {
    [0]=>
    array(14) {
      ["pid"]=>
      int(%d)
      ["state"]=>
      string(7) "Running"
      ["start-time"]=>
      int(%d)
      ["start-since"]=>
      int(%d)
      ["requests"]=>
      int(1)
      ["request-duration"]=>
      int(%d)
      ["request-method"]=>
      string(3) "GET"
      ["request-uri"]=>
      string(%d) "%s"
      ["query-string"]=>
      string(0) ""
      ["request-length"]=>
      int(0)
      ["user"]=>
      string(1) "-"
      ["script"]=>
      string(%d) "%s"
      ["last-request-cpu"]=>
      float(0)
      ["last-request-memory"]=>
      int(0)
    }
  }
}
Test End

Request ok
[%s] NOTICE: Terminating ...
[%s] NOTICE: exiting, bye-bye!
Done
--CLEAN--
<?php
	$logfile = __DIR__.'/php-fpm.log.tmp';
	$srcfile = __DIR__.'/php-fpm.tmp.php';
    @unlink($logfile);
    @unlink($srcfile);
?>
