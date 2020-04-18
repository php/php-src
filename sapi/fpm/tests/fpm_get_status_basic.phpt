--TEST--
FPM: Function fpm_get_status basic test
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = dynamic
pm.max_children = 5
pm.start_servers = 1
pm.min_spare_servers = 1
pm.max_spare_servers = 3
EOT;

$code = <<<EOT
<?php
echo "Test Start\n";
var_dump(fpm_get_status());
echo "Test End\n";
EOT;

$headers = [];
$tester = new FPM\Tester($cfg, $code);
$tester->start();
$tester->expectLogStartNotices();
$tester->request()->printBody();
$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
Done
--EXPECTF--
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
  int(%d)
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
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
